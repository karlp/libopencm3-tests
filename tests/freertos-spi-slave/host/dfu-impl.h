/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * Try and contain the DFU relevant portions in one module.
 */

#pragma once

#include <stdint.h>
#include <libopencm3/usb/usbd.h>

#define USB_DFU_INTERFACE_NUM 1

#ifdef __cplusplus
extern "C" {
#endif

enum usbd_request_return_codes dfu_handle_control_request(usbd_device *usbd_dev,
	struct usb_setup_data *req,
	uint8_t **buf,
	uint16_t *len,
	usbd_control_complete_callback *complete);

extern const struct usb_interface_descriptor dfu_rt_iface;

#ifdef __cplusplus
}
#endif
