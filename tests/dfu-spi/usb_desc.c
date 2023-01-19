/**
 * USB Descriptors.
 * Not everything can be easily split, as interfaces run together,
 * so put all the descriptors in one file together.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <libopencm3/usb/dfu.h>
#include <libopencm3/usb/usbd.h>

#include "usb_desc.h"

const struct usb_dfu_descriptor dfu_rt_function = {
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

static const struct usb_device_descriptor dev = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x0200,
	.bDeviceClass = USB_CLASS_VENDOR,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = BULK_EP_MAXPACKET,
	.idVendor = 0xcafe,
	.idProduct = 0xcaff,
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

static const struct usb_interface_descriptor my_iface_bulk[] = {
	{
		.bLength = USB_DT_INTERFACE_SIZE,
		.bDescriptorType = USB_DT_INTERFACE,
		.bInterfaceNumber = 0,
		.bAlternateSetting = 0,
		.bNumEndpoints = 2,
		.bInterfaceClass = USB_CLASS_VENDOR,
		.iInterface = 5,
		.endpoint = endp_bulk,
	}
};

static const struct usb_interface my_interfaces[] = {
	{
		.num_altsetting = 1,
		.altsetting = my_iface_bulk,
	},
	{
		.num_altsetting = 1,
		.altsetting = &dfu_rt_iface,
	}
};

static const struct usb_config_descriptor config[] = {
	{
		.bLength = USB_DT_CONFIGURATION_SIZE,
		.bDescriptorType = USB_DT_CONFIGURATION,
		.wTotalLength = 0,
		.bNumInterfaces = 2,
		.bConfigurationValue = MY_USB_CFG,
		.iConfiguration = 4, /* string index */
		.bmAttributes = 0x80,
		.bMaxPower = 0x32,
		.interface = my_interfaces,
	},
};

/* more than enough size */
static char serial[20];
static const char *usb_strings[] = {
	"not-libopencm3",
	"dac-controller",
	serial,
	"dac useless config",
	"dac data channel if",
	"DFU interface",
};


usbd_device * usb_start(const usbd_driver *driver, const char *userserial, uint8_t *ctrl_buf, size_t ctrl_buf_len)
{
	if (userserial) {
		strncpy(serial, userserial, sizeof(serial) - 1);
		serial[sizeof(serial)-1] = 0;
	} else {
		sprintf(serial, "fake serial");
	}
	usbd_device *our_dev = usbd_init(driver, &dev, config,
		usb_strings, 6, ctrl_buf, ctrl_buf_len);
	return our_dev;
}
