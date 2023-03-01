/*
 * Just handle the "getting back to the DFU bootloader" portions
 */

#include <stdio.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/rtc.h>

#include <libopencm3/usb/dfu.h>
#include <libopencm3/usb/usbd.h>

#include "dfu-impl.h"

#define ER_DEBUG
#ifdef ER_DEBUG
#define ER_DPRINTF(fmt, ...) \
	do { printf(fmt, ## __VA_ARGS__); } while (0)
#else
#define ER_DPRINTF(fmt, ...) \
	do { } while (0)
#endif

static const struct usb_dfu_descriptor dfu_rt_function = {
	.bLength = sizeof(struct usb_dfu_descriptor),
	.bDescriptorType = DFU_FUNCTIONAL,
	.bmAttributes = USB_DFU_CAN_UPLOAD | USB_DFU_CAN_DOWNLOAD | USB_DFU_WILL_DETACH,
	.wDetachTimeout = 50,
	.wTransferSize = 128,
	/* standard dfu, no st extensions */
	.bcdDFUVersion = 0x0110,
};

const struct usb_interface_descriptor dfu_rt_iface = {
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = USB_DFU_INTERFACE_NUM,
	.bAlternateSetting = 0,
	.bNumEndpoints = 0,
	.bInterfaceClass = 0xFE,
	.bInterfaceSubClass = 1,
	.bInterfaceProtocol = 1,
	.iInterface = 6,

	.extra = &dfu_rt_function,
	.extralen = sizeof(dfu_rt_function),
};

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
		RTC_BKPXR(1) = 0;
	} else {
		RTC_BKPXR(1) = CMD_BOOT;
	}
	pwr_enable_backup_domain_write_protect();
}


static void usb_dfu_detach_complete(usbd_device *dev, struct usb_setup_data *req)
{
	(void) dev;
	(void) req;
	bl_request_boot(false);
	scb_reset_system();
}

enum usbd_request_return_codes dfu_handle_control_request(usbd_device *usbd_dev,
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
