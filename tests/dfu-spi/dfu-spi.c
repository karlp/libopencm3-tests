/*
 * usb to spi master bridge, using freertos
 * Consider to be BSD2 Clause, Apache 2.0, MIT, or ISC licensed, at your
 * pleasure.
 * karl Palsson <karlp@tweak.net.au>
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/usb/dfu.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/iwdg.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/syscfg.h>
#include <libopencm3/stm32/usart.h>

#include "hw.h"
#include "trace.h"
#include "queue.h"

#include "syscfg.h"
#include "funcgen.h"
#include "modbus.h"
//#include "serial_support.h"
//#include "sync.h"
#include "usb_desc.h"

#define ER_DEBUG
#ifdef ER_DEBUG
#define ER_DPRINTF(fmt, ...) \
	do { printf(fmt, ## __VA_ARGS__); } while (0)
#else
#define ER_DPRINTF(fmt, ...) \
	do { } while (0)
#endif

TaskHandle_t taskHandleUSBD;
static struct bar_faker_app_state app_state;

/**
 * Communicate with (potentially) different bootloaders
 * dapboot uses a backup register.
 * @param boot_user true to boot our app code, false for bootloader
 */
static void bl_request_boot(bool boot_user)
{
	static const uint32_t CMD_BOOT = 0x544F4F42UL;
	pwr_disable_backup_domain_write_protect();
	if (boot_user) {
		RTC_BKPXR(0) = 0;
	} else {
		RTC_BKPXR(0) = CMD_BOOT;
	}
	pwr_enable_backup_domain_write_protect();
}


static void task_app(void *pvParameters)
{
	struct bar_faker_app_state *st = pvParameters;
	int i = 0;
	funcgen_init(st);
	//struct sync_state_info sync = {0};
	//sync_setup(&sync);

	while (1) {
		i++;
		printf("dac thread still alive %d\n", i);
		vTaskDelay(portTICK_PERIOD_MS * 5000);
	}
}

/* Buffer to be used for control requests. */
static uint8_t usbd_control_buffer[3*BULK_EP_MAXPACKET];


static void usb_funcgen_out_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void) usbd_dev;
	(void) ep;
	//uint16_t x;
	//x = usbd_ep_read_packet(usbd_dev, ep, dest, BULK_EP_MAXPACKET);
}

static void usb_funcgen_in_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void) usbd_dev;
	(void) ep;
	//uint16_t x = usbd_ep_write_packet(usbd_dev, ep, src, BULK_EP_MAXPACKET);
}


static enum usbd_request_return_codes usb_funcgen_control_request(usbd_device *usbd_dev,
	struct usb_setup_data *req,
	uint8_t **buf,
	uint16_t *len,
	usbd_control_complete_callback *complete)
{
	(void)usbd_dev;
	(void)complete;
	ER_DPRINTF("ctrl breq: %x, bmRT: %x, windex :%x, wlen: %x, wval :%x\n",
		req->bRequest, req->bmRequestType, req->wIndex, req->wLength,
		req->wValue);

	uint8_t *real = *buf;
	/* TODO - what do the return values mean again? */
	/* remember, we're only taking vendor interface reqs here, no need to recheck! */
	float freq;
	float amp;
	float offset;
	uint32_t val;
	switch (req->bRequest) {
	case 1:
		val = *(uint32_t *)&real[0];
		freq = val / 1000.0f;
		val = *(uint32_t *)&real[4];
		amp = val / 1000.0f;
		val = *(uint32_t *)&real[8];
		offset = val / 1000.0f;
		ER_DPRINTF("do sine: freq: %f, amp: %f, offset: %f\n", freq, amp, offset);
		funcgen_sin(req->wValue, freq, amp, offset);
		*len = 0;
		return USBD_REQ_HANDLED;

	case 2:
		funcgen_sin(req->wValue, 50, 0.5f, 0.5f);
		*len = 0;
		return USBD_REQ_HANDLED;

	case 3:
		if (req->wValue) {
			gpio_set(hw_details.led_port, hw_details.led_pin);
		} else {
			gpio_clear(hw_details.led_port, hw_details.led_pin);
		}
		*len = 0;
		return USBD_REQ_HANDLED;
	case 4:
		// TODO - could handle these in one req? would it save anything meaningful?
		funcgen_output(req->wValue, req->wIndex == 1);
		*len = 0;
		return USBD_REQ_HANDLED;
	case 5:
		// Buffer enable
		funcgen_buffer(req->wValue, req->wIndex == 1);
		*len = 0;
		return USBD_REQ_HANDLED;
	case 6:
		for (int i = 0; i < 3; i++) {
			val = *(int32_t *)&real[i*4];
			app_state.zx_phase_adjust[i] = val;
		}
		*len = 0;
		return USBD_REQ_HANDLED;
	case 7:
		val = *(int32_t *)&real[0];
		app_state.base_offset = val;
		*len = 0;
		return USBD_REQ_HANDLED;
	case 100:
		if (req->wValue == 1) {
			bl_request_boot(false);
		} else {
			bl_request_boot(true);
		}
		scb_reset_system();
		/* to keep compilers happy, but unreachable */
		return USBD_REQ_HANDLED;


	default:
		ER_DPRINTF("unexpected control breq: %x deferring?!\n", req->bRequest);
		break;
	}
	return USBD_REQ_NEXT_CALLBACK;
}

static void usb_dfu_detach_complete(usbd_device *dev, struct usb_setup_data *req)
{
	(void)dev;
	(void)req;
	bl_request_boot(false);
	scb_reset_system();
}

static enum usbd_request_return_codes usb_dfu_control_request(usbd_device *usbd_dev,
	struct usb_setup_data *req,
	uint8_t **buf,
	uint16_t *len,
	usbd_control_complete_callback *complete)
{
	(void) usbd_dev;
	(void) complete;
	ER_DPRINTF("ctrl dfu breq: %x, bmRT: %x, windex :%x, wlen: %x, wval :%x\n",
		req->bRequest, req->bmRequestType, req->wIndex, req->wLength,
		req->wValue);

	switch (req->bRequest) {
	case DFU_GETSTATUS:
		if (req->wIndex == USB_DFU_INTERFACE_NUM) {
			(*buf)[0] = DFU_STATUS_OK;
			(*buf)[1] = 0;
			(*buf)[2] = 0;
			(*buf)[3] = 0;
			(*buf)[4] = STATE_APP_IDLE;
			(*buf)[5] = 0; /* no strings for status */
			*len = 6;

			return USBD_REQ_HANDLED;
		}
		break;
	case DFU_DETACH:
		if (req->wIndex == USB_DFU_INTERFACE_NUM) {
			*complete = usb_dfu_detach_complete;
			return USBD_REQ_HANDLED;
		}
		break;
	}
	return USBD_REQ_NEXT_CALLBACK;
}


static void usb_cb_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
	ER_DPRINTF("set cfg %d\n", wValue);
	switch (wValue) {
	case MY_USB_CFG:
		usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, BULK_EP_MAXPACKET,
			usb_funcgen_out_cb);
		usbd_ep_setup(usbd_dev, 0x81, USB_ENDPOINT_ATTR_BULK, BULK_EP_MAXPACKET,
			usb_funcgen_in_cb);
		usbd_register_control_callback(
			usbd_dev,
			USB_REQ_TYPE_VENDOR | USB_REQ_TYPE_INTERFACE,
			USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
			usb_funcgen_control_request);
		usbd_register_control_callback(usbd_dev,
			USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
			USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
			usb_dfu_control_request);
		break;
	default:
		ER_DPRINTF("set configuration unknown: %d\n", wValue);
	}
}


static void prvTaskUSBD(void *pvParameters)
{
	(void)pvParameters;

	/* the original purple bar faker had a hard pullup, the lcom3 hw1 board uses the internal controls. */
#define BETTER_USB_HARDWARE
#if defined(BETTER_USB_HARDWARE)
	/* turn off the usb pull up right now, helps us re-enumerate */
	SYSCFG_PMC &= ~SYSCFG_PMC_USB_PU;
	vTaskDelay(portTICK_PERIOD_MS * 10);

	/* Enable built in USB pullup on L1 */
        rcc_periph_clock_enable(RCC_SYSCFG);
        SYSCFG_PMC |= SYSCFG_PMC_USB_PU;
#endif

#ifdef ER_DEBUG
	setbuf(stdout, NULL);
#endif
	uint16_t mb_compat_serial[3];
	char mb_serial_str[13];
	//setup_serial_number(mb_compat_serial);
	sprintf(mb_serial_str, "%04X%04X%04X", mb_compat_serial[0], mb_compat_serial[1], mb_compat_serial[2]);

	usbd_device *our_dev = usb_start(&st_usbfs_v1_usb_driver, mb_serial_str, usbd_control_buffer, sizeof(usbd_control_buffer));

	usbd_register_set_config_callback(our_dev, usb_cb_set_config);

	/* numerically greater than free rtos kernel split (lower priority) */
	nvic_set_priority(NVIC_USB_LP_IRQ, IRQ2NVIC_PRIOR(6));
	nvic_enable_irq(NVIC_USB_LP_IRQ);

	const TickType_t xBlockTime = pdMS_TO_TICKS( 500 );
	uint32_t ulNotifiedValue;

	ER_DPRINTF("USBD: loop start\n");
	while (1) {

		ulNotifiedValue = ulTaskNotifyTake(pdTRUE, xBlockTime);
		trace_send8(1, ulNotifiedValue);

		if (ulNotifiedValue == 0) {
			/* No big deal, just no usb traffic. just gives us an idle blip */
			ER_DPRINTF(".");
		} else {
			/* ulNotifiedValue holds a count of the number of outstanding
			interrupts.  Process each in turn. */
			while (ulNotifiedValue--) {
				usbd_poll(our_dev);
			}
			nvic_enable_irq(NVIC_USB_LP_IRQ);
		}
	}
}


int main(void)
{
	bl_request_boot(true);
	hw_setup();
	printf("starting bar-faker1...\n");

	xTaskCreate(task_app, "app-main", configMINIMAL_STACK_SIZE, &app_state, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(prvTaskUSBD, "USBD", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &taskHandleUSBD);

	vTaskStartScheduler();

	return 0;
}

void usb_lp_isr(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	nvic_disable_irq(NVIC_USB_LP_IRQ);
	xHigherPriorityTaskWoken = pdFALSE;
	vTaskNotifyGiveFromISR(taskHandleUSBD, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void vAssertCalled(const char * const pcFileName, unsigned long ulLine)
{
	volatile unsigned long ulSetToNonZeroInDebuggerToContinue = 0;

	/* Parameters are not used. */
	(void) ulLine;
	(void) pcFileName;

	taskENTER_CRITICAL();
	{
		while (ulSetToNonZeroInDebuggerToContinue == 0) {
			/* Use the debugger to set ulSetToNonZeroInDebuggerToContinue to a
			non zero value to step out of this function to the point that raised
			this assert(). */
			__asm volatile( "NOP");
			__asm volatile( "NOP");
		}
	}
	taskEXIT_CRITICAL();
}

void vApplicationStackOverflowHook(TaskHandle_t xTask,  signed char *pcTaskName);
void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    signed char *pcTaskName )
{
	volatile unsigned long ulSetToNonZeroInDebuggerToContinue = 0;
	(void) xTask;

	ER_DPRINTF("!!!OVERFLOW in %s\n", pcTaskName);
	taskENTER_CRITICAL();
	{
		while (ulSetToNonZeroInDebuggerToContinue == 0) {
			/* Use the debugger to set ulSetToNonZeroInDebuggerToContinue to a
			non zero value to step out of this function to the point that raised
			this assert(). */
			__asm volatile( "NOP");
			__asm volatile( "NOP");
		}
	}
	taskEXIT_CRITICAL();
}
