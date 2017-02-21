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
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include <stdio.h>
#include "syscfg.h"
#include "usb_cdcacm.h"
#include "ringb.h"
#include "trace.h"

#define ER_DEBUG
#ifdef ER_DEBUG
#define ER_DPRINTF(fmt, ...) \
    do { printf(fmt, ## __VA_ARGS__); } while (0)
#else
#define ER_DPRINTF(fmt, ...) \
    do { } while (0)
#endif

static inline void gpio_really(uint32_t port, uint16_t pin, const bool set)
{
	int shift = set ? 0 : 16;
	GPIO_BSRR(port) = pin << shift;
}

extern struct ringb rx_ring, tx_ring;
static void usart_setup(void)
{
	/* Enable the USART2 interrupt. */
	nvic_enable_irq(NVIC_USART2_IRQ);

	/* USART2 pins are on port A */
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
	gpio_set_af(GPIOA, GPIO_AF7, GPIO2 | GPIO3);

	/* Enable clocks for USART2. */
	rcc_periph_clock_enable(RCC_USART2);

	/* Setup USART2 parameters. */
	usart_set_baudrate(USART2, 115200);
	usart_set_databits(USART2, 8);
	usart_set_stopbits(USART2, USART_STOPBITS_1);
	usart_set_mode(USART2, USART_MODE_TX_RX);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

	/* Enable USART2 Receive interrupt. */
	usart_enable_rx_interrupt(USART2);

	/* Finally enable the USART. */
	usart_enable(USART2);
}

void usart2_isr(void)
{
	gpio_really(GPIOA, GPIO5, 1);
	// usbser-rxne()
	/* Check if we were called because of RXNE. */
	if (usart_get_flag(USART2, USART_SR_RXNE)) {
		gpio_set(LED_RX_PORT, LED_RX_PIN);
		uint8_t c = usart_recv(USART2);
		if (ringb_put(&rx_ring, c)) {
			// good,
		} else {
			// fatal, you should always have drained by now.
			// (when you've got it all ironed out, _actually_
			// just drop and count drops), but not yet...
			ER_DPRINTF("rx buffer full\n");
			while(1);
		}
		gpio_clear(LED_RX_PORT, LED_RX_PIN);
	}
	// usbser-irq-txe()
	if (usart_get_flag(USART2, USART_SR_TXE)) {
		if (ringb_depth(&tx_ring) == 0) {
			// turn off tx empty interrupts, nothing left to send
			usart_disable_tx_interrupt(USART2);
			ER_DPRINTF("OFF\n");
			// Turn on tx complete interrupts, for rs485 de
//			USART_CR1(USART2) |= ~USART_CR1_TCIE;
		} else {
			int c = ringb_get(&tx_ring);
			usart_send(USART2, c);
		}
	}
	// usbser-irq-txc?  rs485 is auto on some devices, but can be emulated anyway
//	if (usart_get_flag(USART2, USART_SR_TC)) {
//		ER_DPRINTF("TC");
//		// turn off the complete irqs, we're done now.
//		USART_SR(USART2) &= ~USART_SR_TC;
//		USART_CR1(USART2) &= ~USART_CR1_TCIE;
//		gpio_clear(LED_TX_PORT, LED_TX_PIN);
//		gpio_clear(RS485DE_PORT, RS485DE_PIN);
//	}
	gpio_really(GPIOA, GPIO5, 0);
}

void usb_cdcacm_setup_pre_arch(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_OTGFS);

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
		GPIO9 | GPIO11 | GPIO12);
	gpio_set_af(GPIOA, GPIO_AF10, GPIO9 | GPIO11 | GPIO12);
	
}

void usb_cdcacm_setup_post_arch(usbd_device *dev)
{
	(void)dev;
}


void cdcacm_arch_pin(int port, enum cdcacm_pin pin, bool set)
{
	(void)port; // TODO if you want to handle multiple ports
	switch (pin) {
		case CDCACM_PIN_LED_TX:
			gpio_really(LED_TX_PORT, LED_TX_PIN, set);
			break;
		case CDCACM_PIN_LED_RX:
			gpio_really(LED_RX_PORT, LED_RX_PIN, set);
			break;
		case CDCACM_PIN_RS485DE:
			gpio_really(RS485DE_PORT, RS485DE_PIN, set);
			break;
		default:
			break;
	}
}

void cdcacm_arch_txirq(int port, bool set) {
	(void)port; //FIXME if you make this multi port
	if (set) {
		usart_enable_tx_interrupt(USART2);
	} else {
		usart_disable_tx_interrupt(USART2);
	}
}

void cdcacm_arch_set_line_state(int port, uint8_t dtr, uint8_t rts)
{
	(void)port; // FIXME if you want multiple ports
	(void) dtr;
	(void) rts;
	// LM4f has an implementation of this if you're keen
}


int main(void)
{
	rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
	ER_DPRINTF("And we're alive!\n");
	/* Leds and rs485 are on port D */
	rcc_periph_clock_enable(RCC_GPIOD);
	gpio_mode_setup(LED_RX_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
		LED_RX_PIN | LED_TX_PIN);
	gpio_mode_setup(RS485DE_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
		RS485DE_PIN);

	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);

	usart_setup();
	usb_cdcacm_setup_pre_arch();
	usbd_device *usbd_dev = usb_cdcacm_init(&otgfs_usb_driver, "stm32f4-disco");
	usb_cdcacm_setup_post_arch(usbd_dev);

	ER_DPRINTF("Looping...\n");
	volatile int i = 0;
	while (1) {
		usbd_poll(usbd_dev);
		usb_cdcacm_poll(usbd_dev);
	}

}

