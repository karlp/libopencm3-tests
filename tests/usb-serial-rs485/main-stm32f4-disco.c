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


static usbd_device *usbd_dev;
static struct ringb rx_ring;
static uint8_t rx_ring_data[64];
struct ringb tx_ring;
static uint8_t tx_ring_data[128];
bool nakked = false;

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
	// usbser-rxne()
	/* Check if we were called because of RXNE. */
	if (usart_get_interrupt_source(USART2, USART_SR_RXNE)) {
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
	if (usart_get_interrupt_source(USART2, USART_SR_TXE)) {
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
//	if (usart_get_interrupt_source(USART2, USART_SR_TC)) {
//		ER_DPRINTF("TC");
//		// turn off the complete irqs, we're done now.
//		USART_SR(USART2) &= ~USART_SR_TC;
//		USART_CR1(USART2) &= ~USART_CR1_TCIE;
//		gpio_clear(LED_TX_PORT, LED_TX_PIN);
//		gpio_clear(RS485DE_PORT, RS485DE_PIN);
//	}
}

/* Y0, moron, nothing's stopping rx irqs from happening, have fun when you overflow temp buffer! */
static void task_drain_rx(struct ringb *r) {
	uint8_t zero_copy_is_for_losers[sizeof(rx_ring_data)];
	int zci = 0;
	int c = ringb_get(r);
	while (c >= 0) {
		zero_copy_is_for_losers[zci++] = c;
		c = ringb_get(r);
	}
	if (zci) {
		trace_send16(STIMULUS_RING_DRAIN, zci);
		glue_data_received_cb(zero_copy_is_for_losers, zci);
	}

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

	usart_setup();
	ringb_init(&rx_ring, rx_ring_data, sizeof(rx_ring_data));
	ringb_init(&tx_ring, tx_ring_data, sizeof(tx_ring_data));

	usb_cdcacm_init(&usbd_dev);

	ER_DPRINTF("Looping...\n");
	volatile int i = 0;
	while (1) {
		usbd_poll(usbd_dev);
		if (i++ > 500) {
			// hacktastic
			if (ringb_depth(&tx_ring) < 64 && nakked) {
				usbd_ep_nak_set(usbd_dev, 1, 0);
				nakked = false;
			}

			task_drain_rx(&rx_ring);
			i = 0;
		}

	}

}

