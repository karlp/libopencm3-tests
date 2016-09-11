/*
 * Oct 2015 Karl Palsson <karlp@tweak.net.au>
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include "trace.h"
#include "adc-power.h"

#define LED_PORT GPIOC
#define LED_PIN GPIO13

int main(void)
{
	int i;
	int j = 0;
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
	rcc_periph_clock_enable(RCC_GPIOC);
	printf("hi guys!\n");
	/* green led for ticking */
	gpio_set_mode(LED_PORT, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, LED_PIN);

	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO0);
        gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO5);


	adc_power_init();
	while (1) {
		adc_power_task_up();
		gpio_toggle(LED_PORT, LED_PIN);

		for (i = 0; i < 0x80000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
		adc_power_task_down();
		gpio_toggle(LED_PORT, LED_PIN);
		for (i = 0; i < 0x80000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
	}

	return 0;
}
