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
#include "queue.h"

#define ER_DEBUG
#ifdef ER_DEBUG
#define ER_DPRINTF(fmt, ...) \
	do { printf(fmt, ## __VA_ARGS__); } while (0)
#else
#define ER_DPRINTF(fmt, ...) \
	do { } while (0)
#endif

TaskHandle_t taskHandleUSBD;
QueueHandle_t spiQ_rx;

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
	/* Might be software, might be hardware controlled */
	.cs_rcc = RCC_GPIOB,
	.cs_port = GPIOB,
	.cs_pin = GPIO12,
	.led_port = GPIOB,
	.led_pin = GPIO8,
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
	/* Make sure we're starting clean */
	gpio_clear(hw_details.trigger_port, hw_details.trigger_pin);

	/* "spare" "led" line */
	gpio_mode_setup(hw_details.led_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, hw_details.led_pin);

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
		vTaskDelay(portTICK_PERIOD_MS * 500);
		gpio_toggle(hw_details.led_port, hw_details.led_pin);
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
	/* This is _heaps_ */
	/* NOTE: queues not stream/message buffers because freertos 10.x is busted in openocd, no other reason */
	spiQ_rx = xQueueCreate(1024, 1);
	configASSERT(spiQ_rx);
#if 0
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
#endif
	/* FIXME - this task only necessary for handling spi slave mode
	 * or potentially, if I get freaky, running transnfers registered by usb modes?
	 * Start with doing nothing!
	 */
	while(1) {
		vTaskDelay(portTICK_PERIOD_MS * 1000);
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

/**
 * Cleanly disable the peripheral.
 * Use this after your last spi_xfer to disable the peripheral.
 * Important if you are using hardware NSS in output
 * @param spi peripheral of choice
 * @sa spi_xfer
 */
static void spi_clean_disable2(uint32_t spi) {
	/* Wait to transmit last data */
	while (!(SPI_SR(spi) & SPI_SR_TXE));

	/* Wait until not busy */
	while (SPI_SR(spi) & SPI_SR_BSY);

	SPI_CR1(spi) &= ~SPI_CR1_SPE;	
}

static void hostspi_out_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void) usbd_dev;
	(void) ep;
	//uint16_t x;
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
	(void)usbd_dev;
	(void)complete;
	ER_DPRINTF("ctrl breq: %x, bmRT: %x, windex :%x, wlen: %x, wval :%x\n",
		req->bRequest, req->bmRequestType, req->wIndex, req->wLength,
		req->wValue);

	uint8_t *real = *buf;
	/* TODO - what do the return values mean again? */
	/* remember, we're only taking vendor interface reqs here, no need to recheck! */
	switch (req->bRequest) {
	case 1:
		/* (re)init master */
		if (req->wLength != 5) {
			ER_DPRINTF("Illegal number of args %d != 5\n", req->wLength);
			return USBD_REQ_NOTSUPP;
		}
		rcc_periph_reset_pulse(hw_details.periph_rst);
		/* chip select as a regular GPIO for this mode. assume RCC already setup */
		gpio_mode_setup(hw_details.cs_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, hw_details.cs_pin);
		gpio_set(hw_details.cs_port, hw_details.cs_pin);

		/* yes, there's enough bytes in the control request itself, but bandwidth optimization why?! */
		spi_init_master(hw_details.periph, real[0], real[1], real[2], real[3], real[4]);
		/* Ignore the stupid NSS pin. */
		spi_enable_software_slave_management(hw_details.periph);
		spi_set_nss_high(hw_details.periph);

		/* Finally enable the SPI. */
		spi_enable(hw_details.periph);
		*len = 0;
		return USBD_REQ_HANDLED;

	case 2:
		/* init with hardware cs mode */
		if (req->wLength != 5) {
			ER_DPRINTF("Illegal number of args %d != 5\n", req->wLength);
			return USBD_REQ_NOTSUPP;
		}
		rcc_periph_reset_pulse(hw_details.periph_rst);
		/* hardware spi chip select needs to reset this to AF */
		gpio_set_af(hw_details.cs_port, GPIO_AF5, hw_details.cs_pin);
		gpio_mode_setup(hw_details.cs_port, GPIO_MODE_AF, GPIO_PUPD_NONE, hw_details.cs_pin);

		/* yes, there's enough bytes in the control request itself, but bandwidth optimization why?! */
		spi_init_master(hw_details.periph, real[0], real[1], real[2], real[3], real[4]);
		*len = 0;
		return USBD_REQ_HANDLED;

	case 3:
		if (real[0]) {
			gpio_set(hw_details.trigger_port, hw_details.trigger_pin);
		} else {
			gpio_clear(hw_details.trigger_port, hw_details.trigger_pin);
		}
		*len = 0;
		return USBD_REQ_HANDLED;

	/* FIXME - both of these run completely blocking within the usb task thread.
	 * Especially if you want to try slow speeds, make them work async in the spi thread instead?
	 */
	case 10:
		/* spi xfer */
		/* If the user/host chose not to read out the reply buffer, then they lose it
		 * we're making a new transfer now. */
		if (uxQueueMessagesWaiting(spiQ_rx) > 0) {
			ER_DPRINTF("Tossing old rx data\n");
		}
		xQueueReset(spiQ_rx);
		gpio_clear(hw_details.cs_port, hw_details.cs_pin);
		for (int i = 0; i < req->wLength; i++) {
			uint8_t x = spi_xfer(hw_details.periph, real[i]);
			BaseType_t rv = xQueueSendToBack(spiQ_rx, &x, 0);
			// If you overflowed this jumbo queue, you have issues
			configASSERT(rv == pdTRUE);
		}
		gpio_set(hw_details.cs_port, hw_details.cs_pin);
		return USBD_REQ_HANDLED;
	case 11:
		/* Read xfer buffer */
		/* You should call this after issuing a spi transfer req if you want to see what came back! */
		/* Lengths are good thankfully, it's spi, queue is as long as they transferred. */
		if (req->wLength != uxQueueMessagesWaiting(spiQ_rx)) {
			ER_DPRINTF("requested read not matching our stored length? %lu\n", uxQueueMessagesWaiting(spiQ_rx));
			return USBD_REQ_NOTSUPP;
		}
		for (int i = 0; i < req->wLength; i++) {
			if (!xQueueReceive(spiQ_rx, &real[i], 0)) {
				ER_DPRINTF("Couldn't read?!\n");
				configASSERT(false);
			}
		}
		*len = req->wLength;
		return USBD_REQ_HANDLED;

	case 12:
		/* write, the other way.... */
		if (uxQueueMessagesWaiting(spiQ_rx) > 0) {
			ER_DPRINTF("Tossing old rx data\n");
		}
		xQueueReset(spiQ_rx);
		spi_enable(hw_details.periph);
		for (int i = 0; i < req->wLength; i++) {
			uint8_t x = spi_xfer(hw_details.periph, real[i]);
			BaseType_t rv = xQueueSendToBack(spiQ_rx, &x, 0);
			// If you overflowed this jumbo queue, you have issues
			configASSERT(rv == pdTRUE);
		}
		spi_clean_disable2(hw_details.periph);
		return USBD_REQ_HANDLED;

	default:
		ER_DPRINTF("unexpected control breq: %x deferring?!\n", req->bRequest);
		break;
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
		break;
	default:
		ER_DPRINTF("set configuration unknown: %d\n", wValue);
	}
}


static void prvTaskUSBD(void *pvParameters)
{
	(void)pvParameters;
	//gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8);
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
				//gpio_set(GPIOB, GPIO8);
				usbd_poll(our_dev);
				//gpio_clear(GPIOB, GPIO8);
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

