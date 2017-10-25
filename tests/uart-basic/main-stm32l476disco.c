/*
 * Oct 2017 Karl Palsson <karlp@tweak.net.au>
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include "uart-basic.h"

#define LED_DISCO_GREEN_RCC RCC_GPIOE
#define LED_DISCO_GREEN_PORT GPIOE
#define LED_DISCO_GREEN_PIN GPIO8

void usart2_isr(void)
{
	ub_irq_handler();
}

static void setup_rcc_hack(void)
{
	pwr_set_vos_scale(PWR_SCALE1);
	flash_set_ws(FLASH_ACR_LATENCY_2WS);
	rcc_set_msi_range(RCC_CR_MSIRANGE_48MHZ);

	/* ok, we manually poked around, let the lib know */
	rcc_apb1_frequency = rcc_apb2_frequency = 48e6;
}

static void board_init(void)
{
	rcc_periph_clock_enable(RCC_GPIOD);
	gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO5 | GPIO6);
	/* usart2 is AF7 */
	gpio_set_af(GPIOD, GPIO_AF7, GPIO5 | GPIO6);
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
