#pragma once

#define MY_USB_CFG 2
#define BULK_EP_MAXPACKET	64
#define USB_DFU_INTERFACE_NUM 1

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 *
	 * @param driver
	 * @param userserial optional, string to provide as device serial
	 * @param ctrl_buf
	 * @param ctrl_buf_len
	 * @return
	 */
	usbd_device * usb_start(const usbd_driver *driver, const char *userserial,
		uint8_t *ctrl_buf, size_t ctrl_buf_len);


#ifdef __cplusplus
}
#endif
