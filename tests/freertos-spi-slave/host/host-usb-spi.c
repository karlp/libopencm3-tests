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
#include <libopencm3/usb/usbd.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/syscfg.h>

#include "hw.h"
#include "trace.h"

#define ER_DEBUG
#ifdef ER_DEBUG
#define ER_DPRINTF(fmt, ...) \
	do { printf(fmt, ## __VA_ARGS__); } while (0)
#else
#define ER_DPRINTF(fmt, ...) \
	do { } while (0)
#endif

TaskHandle_t taskHandleUSBD;

struct hw_detail hw_details = {
	.periph = SPI2,
	.periph_rcc = RCC_SPI2,
	.periph_rst = RST_SPI2,
	.pins = GPIO13 | GPIO14 | GPIO15, /* SPI pins for setting AF with */
	.port = GPIOB,
	.port_rcc = RCC_GPIOB,
	.trigger_rcc = RCC_GPIOB,
	.trigger_port = GPIOB,
	.trigger_pin = GPIO9,
};

/* provided in board files please*/
/**
 * Setup any gpios or anything hardware specific.
 * Should _only_ be things that can't be done in shared init()
 */
static void hw_init(void)
{
	/* trigger pin gpio */
	rcc_periph_clock_enable(hw_details.trigger_rcc);
	gpio_mode_setup(hw_details.trigger_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, hw_details.trigger_pin);

	/* spi control lines */
	rcc_periph_clock_enable(hw_details.port_rcc);
	gpio_mode_setup(hw_details.port, GPIO_MODE_AF, GPIO_PUPD_NONE, hw_details.pins);
	gpio_set_output_options(hw_details.port, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, hw_details.pins);
	gpio_set_af(hw_details.port, GPIO_AF5, hw_details.pins);
}


static void prvTaskGreenBlink1(void *pvParameters)
{
	(void) pvParameters;
	int i = 0;
	while (1) {
		printf("gblink %d\n", i++);
		vTaskDelay(portTICK_PERIOD_MS * 550);
		gpio_toggle(hw_details.trigger_port, hw_details.trigger_pin);
	}

	/* Tasks must not attempt to return from their implementing
	function or otherwise exit.  In newer FreeRTOS port
	attempting to do so will result in an configASSERT() being
	called if it is defined.  If it is necessary for a task to
	exit then have the task call vTaskDelete( NULL ) to ensure
	its exit is clean. */
	vTaskDelete(NULL);
}

static void prvTaskSpiMaster(void *pvParameters)
{
	(void)pvParameters;
        rcc_periph_clock_enable(hw_details.periph_rcc);
        spi_init_master(hw_details.periph, SPI_CR1_BAUDRATE_FPCLK_DIV_32, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
        /* Ignore the stupid NSS pin. */
        spi_enable_software_slave_management(hw_details.periph);
        spi_set_nss_high(hw_details.periph);

        /* Finally enable the SPI. */
        spi_enable(hw_details.periph);

	int i = 0;
	while (1) {
		printf("spi send %d\n", i);
		vTaskDelay(portTICK_PERIOD_MS * 200);
	        spi_xfer(hw_details.periph, 0xaa);
	        spi_xfer(hw_details.periph, i++);
	}

}


/* USB configurations */
#define GZ_CFG_SOURCESINK	2

#define BULK_EP_MAXPACKET	64

static const struct usb_device_descriptor dev = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x0200,
	.bDeviceClass = USB_CLASS_VENDOR,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = BULK_EP_MAXPACKET,
	.idVendor = 0xcafe,
	.idProduct = 0xcafe,
	.bcdDevice = 0x0001,
	.iManufacturer = 1,
	.iProduct = 2,
	.iSerialNumber = 3,
	.bNumConfigurations = 1,
};

static const struct usb_endpoint_descriptor endp_bulk[] = {
	{
		.bLength = USB_DT_ENDPOINT_SIZE,
		.bDescriptorType = USB_DT_ENDPOINT,
		.bEndpointAddress = 0x01,
		.bmAttributes = USB_ENDPOINT_ATTR_BULK,
		.wMaxPacketSize = BULK_EP_MAXPACKET,
		.bInterval = 1,
	},
	{
		.bLength = USB_DT_ENDPOINT_SIZE,
		.bDescriptorType = USB_DT_ENDPOINT,
		.bEndpointAddress = 0x81,
		.bmAttributes = USB_ENDPOINT_ATTR_BULK,
		.wMaxPacketSize = BULK_EP_MAXPACKET,
		.bInterval = 1,
	},
};

static const struct usb_interface_descriptor iface_sourcesink[] = {
	{
		.bLength = USB_DT_INTERFACE_SIZE,
		.bDescriptorType = USB_DT_INTERFACE,
		.bInterfaceNumber = 0,
		.bAlternateSetting = 0,
		.bNumEndpoints = 2,
		.bInterfaceClass = USB_CLASS_VENDOR,
		.iInterface = 0,
		.endpoint = endp_bulk,
	}
};

static const struct usb_interface ifaces_sourcesink[] = {
	{
		.num_altsetting = 1,
		.altsetting = iface_sourcesink,
	}
};

static const struct usb_config_descriptor config[] = {
	{
		.bLength = USB_DT_CONFIGURATION_SIZE,
		.bDescriptorType = USB_DT_CONFIGURATION,
		.wTotalLength = 0,
		.bNumInterfaces = 1,
		.bConfigurationValue = GZ_CFG_SOURCESINK,
		.iConfiguration = 4, /* string index */
		.bmAttributes = 0x80,
		.bMaxPower = 0x32,
		.interface = ifaces_sourcesink,
	},
};

static char serial[] = "0123456789.0123456789.0123456789";
static const char *usb_strings[] = {
	"libopencm3",
	"spi-host-freertos",
	serial,
	"source and sink data",
};

/* Buffer to be used for control requests. */
static uint8_t usbd_control_buffer[5*BULK_EP_MAXPACKET];


static void hostspi_out_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void) usbd_dev;
	(void) ep;
	uint16_t x;
	//x = usbd_ep_read_packet(usbd_dev, ep, dest, BULK_EP_MAXPACKET);
}

static void hostspi_in_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void) usbd_dev;
	(void) ep;
	//uint16_t x = usbd_ep_write_packet(usbd_dev, ep, src, BULK_EP_MAXPACKET);
}


static enum usbd_request_return_codes hostspit_control_request(usbd_device *usbd_dev,
	struct usb_setup_data *req,
	uint8_t **buf,
	uint16_t *len,
	usbd_control_complete_callback *complete)
{
	(void) usbd_dev;
	(void) complete;
	(void) buf;
	(void) len;
	ER_DPRINTF("ctrl breq: %x, bmRT: %x, windex :%x, wlen: %x, wval :%x\n",
		req->bRequest, req->bmRequestType, req->wIndex, req->wLength,
		req->wValue);

	/* TODO - what do the return values mean again? */
	switch (req->bRequest) {
	case 42:
		return USBD_REQ_HANDLED;
	case 43:
		return USBD_REQ_NOTSUPP;
	default:
		ER_DPRINTF("Unhandled request!\n");
		return USBD_REQ_NOTSUPP;
	}
	return USBD_REQ_NEXT_CALLBACK;
}

static void hostspi_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
	ER_DPRINTF("set cfg %d\n", wValue);
	switch (wValue) {
	case GZ_CFG_SOURCESINK:
		usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, BULK_EP_MAXPACKET,
			hostspi_out_cb);
		usbd_ep_setup(usbd_dev, 0x81, USB_ENDPOINT_ATTR_BULK, BULK_EP_MAXPACKET,
			hostspi_in_cb);
		usbd_register_control_callback(
			usbd_dev,
			USB_REQ_TYPE_VENDOR | USB_REQ_TYPE_INTERFACE,
			USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
			hostspit_control_request);
		/* Prime source for IN data. */
		// gadget0_ss_in_cb(usbd_dev, 0x81);
		break;
	default:
		ER_DPRINTF("set configuration unknown: %d\n", wValue);
	}
}


static void prvTaskUSBD(void *pvParameters)
{
	(void)pvParameters;
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8);
	/* Enable built in USB pullup on L1 */
        rcc_periph_clock_enable(RCC_SYSCFG);
        SYSCFG_PMC |= SYSCFG_PMC_USB_PU;

#ifdef ER_DEBUG
	setbuf(stdout, NULL);
#endif
	static const char *userserial = "myserial";
	if (userserial) {
		usb_strings[2] = userserial;
	}
	usbd_device *our_dev = usbd_init(&st_usbfs_v1_usb_driver, &dev, config,
		usb_strings, 5,
		usbd_control_buffer, sizeof(usbd_control_buffer));

	usbd_register_set_config_callback(our_dev, hostspi_set_config);

	/* numerically greater than free rtos kernel split (lower priority) */
	nvic_set_priority(NVIC_USB_LP_IRQ, 6<<4);
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
				gpio_set(GPIOB, GPIO8);
				usbd_poll(our_dev);
				gpio_clear(GPIOB, GPIO8);
			}
			nvic_enable_irq(NVIC_USB_LP_IRQ);
		}
	}
}


int main(void)
{
        const struct rcc_clock_scale myclock = {
                .pll_source = RCC_CFGR_PLLSRC_HSE_CLK,
                .pll_mul = RCC_CFGR_PLLMUL_MUL6,
                .pll_div = RCC_CFGR_PLLDIV_DIV3,
                .hpre = RCC_CFGR_HPRE_SYSCLK_NODIV,
                .ppre1 = RCC_CFGR_PPRE1_HCLK_NODIV,
                .ppre2 = RCC_CFGR_PPRE2_HCLK_NODIV,
                .voltage_scale = PWR_SCALE1,
                .flash_waitstates = 1,
                .ahb_frequency = 32e6,
                .apb1_frequency = 32e6,
                .apb2_frequency = 32e6,
        };
        rcc_clock_setup_pll(&myclock);
	hw_init();
	printf("starting freertos...\n");

	xTaskCreate(prvTaskGreenBlink1, "green.blink", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(prvTaskSpiMaster, "spi.master", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
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

