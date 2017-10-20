/*
 * Karl Palsson <karlp@tweak.net.au> Oct 2017
 * Considered to be available under your choice of:
 * BSD2 clause, Apache2, MIT, X11 or ISC licenses
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include "uart-basic.h"

/* prototype to make linking happy */
int _write(int file, char *ptr, int len);

static struct ub_hw *ub;
static uint8_t last_rxb;

/* Implement _write for newlib to use printf */
int _write(int file, char *ptr, int len)
{
	int i;

	if (file == STDOUT_FILENO || file == STDERR_FILENO) {
		for (i = 0; i < len; i++) {
			if (ptr[i] == '\n') {
				usart_send_blocking(ub->uart, '\r');
			}
			usart_send_blocking(ub->uart, ptr[i]);
		}
		return i;
	}
	errno = EIO;
	return -1;
}

void ub_init(struct ub_hw *ub_input)
{
	ub = ub_input;
	rcc_periph_clock_enable(ub->uart_rcc);

	usart_set_baudrate(ub->uart, 115200);
	usart_set_databits(ub->uart, 9);
	usart_set_stopbits(ub->uart, USART_STOPBITS_1);
	usart_set_mode(ub->uart, USART_MODE_TX_RX);
	usart_set_parity(ub->uart, USART_PARITY_EVEN);
	usart_set_flow_control(ub->uart, USART_FLOWCONTROL_NONE);

	usart_enable_rx_interrupt(ub->uart);
	nvic_enable_irq(ub->uart_nvic);
	usart_enable(ub->uart);
}

void ub_task(void)
{
	if (last_rxb) {
		printf("Last rx char was: <%c>\n", last_rxb);
		last_rxb = 0;
	} else {
		printf("enter a character!\n");
	}
}

void ub_irq_handler(void)
{
	if (usart_get_flag(ub->uart, USART_SR_RXNE)) {
		last_rxb = usart_recv(ub->uart);
	}
}
