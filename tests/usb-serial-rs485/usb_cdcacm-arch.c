/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2014 Karl Palsson <karlp@tweak.net.au>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include "usb_cdcacm.h"
#include "syscfg.h"
#include "ringb.h"

extern bool out_in_progress;

void usb_cdcacm_setup_pre_arch(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_OTGFS);

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
		GPIO9 | GPIO11 | GPIO12);
	gpio_set_af(GPIOA, GPIO_AF10, GPIO9 | GPIO11 | GPIO12);

}

void usb_cdcacm_setup_post_arch(void)
{
}

// hacktastic
extern struct ringb tx_ring;
void glue_send_data_cb(uint8_t *buf, uint16_t len)
{
	if (len == 0) {
		return;
	}
	gpio_set(LED_TX_PORT, LED_TX_PIN);
	gpio_set(RS485DE_PORT, RS485DE_PIN);
	for (int x = 0; x < len; x++) {
		if (!ringb_put(&tx_ring, buf[x])) {
			// failed to process usb traffic properly.
			// should _never_ happen, means we failed to nak in time.
			// this is _never_recoverable beyond watchdog reset.
			while(1);
		}
		usart_enable_tx_interrupt(USART2);
	}
}

void glue_set_line_state_cb(uint8_t dtr, uint8_t rts)
{
	(void) dtr;
	(void) rts;
	// LM4f has an implementation of this if you're keen
}

int glue_set_line_coding_cb(uint32_t baud, uint8_t databits,
	enum usb_cdc_line_coding_bParityType cdc_parity,
	enum usb_cdc_line_coding_bCharFormat cdc_stopbits)
{
	int uart_parity;
	int uart_stopbits;

	if (databits < 8 || databits > 9) {
		return 0;
	}

	/* Be careful here, ST counts parity as a data bit */
	switch (cdc_parity) {
	case USB_CDC_NO_PARITY:
		uart_parity = USART_PARITY_NONE;
		break;
	case USB_CDC_ODD_PARITY:
		uart_parity = USART_PARITY_ODD;
		databits++;
		break;
	case USB_CDC_EVEN_PARITY:
		uart_parity = USART_PARITY_EVEN;
		databits++;
		break;
	default:
		return 0;
	}

	switch (cdc_stopbits) {
	case USB_CDC_1_STOP_BITS:
		uart_stopbits = USART_STOPBITS_1;
		break;
	case USB_CDC_2_STOP_BITS:
		uart_stopbits = USART_STOPBITS_2;
		break;
	default:
		return 0;
	}

	/* Disable the UART while we mess with its settings */
	usart_disable(USART2);
	/* Set communication parameters */
	usart_set_baudrate(USART2, baud);
	usart_set_databits(USART2, databits);
	usart_set_parity(USART2, uart_parity);
	usart_set_stopbits(USART2, uart_stopbits);
	/* Back to work. */
	usart_enable(USART2);

	return 1;
}
