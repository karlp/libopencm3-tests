
/*
 * Nov 2015 Karl Palsson <karlp@tweak.net.au>
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/rcc.h>

#include "trace.h"
#include "adc-power.h"

#define LED_DISCO_RED_PORT GPIOB
#define LED_DISCO_RED_PIN GPIO2
#define LED_DISCO_GREEN_PORT GPIOE
#define LED_DISCO_GREEN_PIN GPIO8

int hack(void) {
	pwr_set_vos_scale(PWR_SCALE1);
	flash_set_ws(FLASH_ACR_LATENCY_2WS);
	rcc_set_msi_range(RCC_CR_MSIRANGE_48MHZ);
	return 0;
}

int main(void)
{
	int i;
	int j = 0;
	rcc_periph_clock_enable(RCC_GPIOB); // led
	rcc_periph_clock_enable(RCC_GPIOE); // led
	rcc_periph_clock_enable(RCC_GPIOA); // adcs
	rcc_periph_clock_enable(RCC_GPIOC); // adcs
	rcc_periph_clock_enable(RCC_PWR);
	hack();
	printf("hi guys!\n");
	/* green led for ticking */
	gpio_mode_setup(LED_DISCO_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
			LED_DISCO_GREEN_PIN);
	gpio_mode_setup(LED_DISCO_RED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
			LED_DISCO_RED_PIN);

	/* ADC 1 channels 1 and 5 */
	// This is the reset state, so not really necessary.
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO2);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO3);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO5);

	// This is NOT the reset state! very important!
	GPIO_ASCR(GPIOA) |= GPIO0 | GPIO1 | GPIO2 | GPIO3 | GPIO5;

	adc_power_init();
	while (1) {
		adc_power_task_up();
		gpio_toggle(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);

		for (i = 0; i < 0x10000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
		adc_power_task_down();
		gpio_toggle(LED_DISCO_RED_PORT, LED_DISCO_RED_PIN);
		for (i = 0; i < 0x10000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
	}

	return 0;
}
