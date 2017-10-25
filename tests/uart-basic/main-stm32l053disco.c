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

#define LED_DISCO_GREEN_RCC RCC_GPIOB
#define LED_DISCO_GREEN_PORT GPIOB
#define LED_DISCO_GREEN_PIN GPIO4

void usart1_isr(void)
{
	ub_irq_handler();
}

static void setup_rcc_hack(void)
{
	/* FIXME L0 doesn't have rcc setup helpers (yet) */
	rcc_osc_on(RCC_HSI16);
	rcc_wait_for_osc_ready(RCC_HSI16);
	rcc_set_sysclk_source(RCC_HSI16);

	/* HSI48 needs the vrefint turned on */
	rcc_periph_clock_enable(RCC_SYSCFG);
	SYSCFG_CFGR3 |= SYSCFG_CFGR3_ENREF_HSI48 | SYSCFG_CFGR3_EN_VREFINT;
	while (!(SYSCFG_CFGR3 & SYSCFG_CFGR3_REF_HSI48_RDYF));

	/* For USB, but can't use HSI48 as a sysclock on L0 */
	crs_autotrim_usb_enable();
	rcc_set_hsi48_source_rc48();

	rcc_osc_on(RCC_HSI48);
	rcc_wait_for_osc_ready(RCC_HSI48);

	/* ok, we manually poked around, let the lib know */
	rcc_apb1_frequency = rcc_apb2_frequency = 16e6;
}

static void board_init(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9 | GPIO10);
	/* usart1 is AF4 */
	gpio_set_af(GPIOA, GPIO_AF4, GPIO9 | GPIO10);
}

int main(void)
{
	int i;
	int j = 0;
	setup_rcc_hack();
	board_init();
	struct ub_hw ub = {
		.uart = USART1,
		.uart_nvic = NVIC_USART1_IRQ,
		.uart_rcc = RCC_USART1,
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
