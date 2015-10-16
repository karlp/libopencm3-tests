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

#define LED_DISCO_GREEN_PORT GPIOD
#define LED_DISCO_GREEN_PIN GPIO12

int main(void)
{
	int i;
	int j = 0;
	rcc_clock_setup_hse_3v3(&hse_8mhz_3v3[CLOCK_3V3_168MHZ]);
	rcc_periph_clock_enable(RCC_GPIOD);
	printf("hi guys!\n");
	/* green led for ticking */
	gpio_mode_setup(LED_DISCO_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
			LED_DISCO_GREEN_PIN);

	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1);

	adc_power_init();
	while (1) {
		adc_power_task_up();
		gpio_toggle(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);

		for (i = 0; i < 0x1000000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
		adc_power_task_down();
		gpio_toggle(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);
		for (i = 0; i < 0x1000000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
	}

	return 0;
}
