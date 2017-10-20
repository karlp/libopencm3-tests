/*
 * Oct 2017 Karl Palsson <karlp@tweak.net.au>
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include "uart-basic.h"

#define LED_DISCO_GREEN_PORT GPIOD
#define LED_DISCO_GREEN_PIN GPIO12

void usart2_isr(void)
{
	ub_irq_handler();	
}

static void board_init(void) {
	rcc_periph_clock_enable(RCC_GPIOA);
        gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2|GPIO3);
	/* usart is AF7 */
        gpio_set_af(GPIOA, GPIO_AF7, GPIO2|GPIO3);
}

int main(void)
{
	int i;
	int j = 0;
	rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
	rcc_periph_clock_enable(RCC_GPIOD);
	board_init();
	struct ub_hw ub = {
		.uart = USART2,
		.uart_nvic = NVIC_USART2_IRQ,
		.uart_rcc = RCC_USART2,
	};
	ub_init(&ub);
	printf("hi guys!\n");
	/* green led for ticking */
	gpio_mode_setup(LED_DISCO_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
			LED_DISCO_GREEN_PIN);


	while (1) {
		gpio_toggle(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);

		for (i = 0; i < 0x800000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
		ub_task();
		gpio_toggle(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);
		for (i = 0; i < 0x800000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
	}

	return 0;
}
