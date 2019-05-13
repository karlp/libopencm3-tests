/*
 * Oct 2015 Karl Palsson <karlp@tweak.net.au>
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include "trace.h"
#include "adc-power.h"

static void setup_clocks(void)
{
	rcc_clock_setup_pll(&rcc_hse8mhz_configs[RCC_CLOCK_HSE8_72MHZ]);
}

int main(void)
{
	int i;
	setup_clocks();
	/* Board led */
	rcc_periph_clock_enable(RCC_GPIOE);
	gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8);
	gpio_set(GPIOE, GPIO8);
	printf("hi guys!\n");

	// ADC seems to take these no matter what?
//	rcc_periph_clock_enable(RCC_GPIOA);
//	rcc_periph_clock_enable(RCC_GPIOF);
//	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0);
//	gpio_mode_setup(GPIOF, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO4);

	adc_power_init();
	for (i = 0; i < 0x1000; i++) { /* need as much as 10 usecs for vreg */
		__asm__("NOP");
	}
	while (1) {
		adc_power_task_up();
		gpio_toggle(GPIOE, GPIO8);

		for (i = 0; i < 0x100000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
		printf("tick...\n");
		adc_power_task_down();
		gpio_toggle(GPIOE, GPIO8);
		for (i = 0; i < 0x100000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
	}

	return 0;
}
