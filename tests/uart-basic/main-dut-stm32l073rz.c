/*
 * Oct 2017 Karl Palsson <karlp@tweak.net.au>
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/crs.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/syscfg.h>
#include <libopencm3/stm32/usart.h>

#include "uart-basic.h"

#define LED_NUC_RCC RCC_GPIOA
#define LED_NUC_PORT GPIOA
#define LED_NUC_PIN GPIO5

void usart2_isr(void)
{
	ub_irq_handler();
}

static void setup_rcc_hack(void)
{
	/* FIXME L0 doesn't have rcc setup helpers (yet) */
	rcc_osc_on(RCC_HSI16);
	rcc_wait_for_osc_ready(RCC_HSI16);
	rcc_set_sysclk_source(RCC_HSI16);

	/* ok, we manually poked around, let the lib know */
	rcc_apb1_frequency = rcc_apb2_frequency = 16e6;
}

static void board_init(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
	/* usart is AF4 */
	gpio_set_af(GPIOA, GPIO_AF4, GPIO2 | GPIO3);
}

int main(void)
{
	int i;
	int j = 0;
	setup_rcc_hack();
	board_init();
	struct ub_hw ub = {
		.uart = USART2,
		.uart_nvic = NVIC_USART2_IRQ,
		.uart_rcc = RCC_USART2,
	};
	ub_init(&ub);
	printf("hi guys!\n");
	/* green led for ticking */
	rcc_periph_clock_enable(LED_NUC_RCC);
	gpio_mode_setup(LED_NUC_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
		LED_NUC_PIN);


	while (1) {
		gpio_toggle(LED_NUC_PORT, LED_NUC_PIN);

		for (i = 0; i < 0x40000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
		ub_task();
		gpio_toggle(LED_NUC_PORT, LED_NUC_PIN);
		for (i = 0; i < 0x40000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
	}

	return 0;
}
