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

#define LED_DISCO_GREEN_RCC RCC_GPIOC
#define LED_DISCO_GREEN_PORT GPIOC
#define LED_DISCO_GREEN_PIN GPIO9

void usart3_4_isr(void)
{
	ub_irq_handler();	
}

static void board_init(void) {
	rcc_periph_clock_enable(RCC_GPIOB);
        gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10|GPIO11);
	/* usart3 is AF4 */
        gpio_set_af(GPIOB, GPIO_AF4, GPIO10|GPIO11);
}

int main(void)
{
	int i;
	int j = 0;
	rcc_clock_setup_in_hsi48_out_48mhz();
	board_init();
	struct ub_hw ub = {
		.uart = USART3,
		.uart_nvic = NVIC_USART3_4_IRQ,
		.uart_rcc = RCC_USART3,
	};
	ub_init(&ub);
	printf("hi guys!\n");
	/* green led for ticking */
	rcc_periph_clock_enable(LED_DISCO_GREEN_RCC);
	gpio_mode_setup(LED_DISCO_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
			LED_DISCO_GREEN_PIN);


	while (1) {
		gpio_toggle(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);

		for (i = 0; i < 0xa0000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
		ub_task();
		gpio_toggle(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);
		for (i = 0; i < 0xa0000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
	}

	return 0;
}
