
/*
 * Nov 2015 Karl Palsson <karlp@tweak.net.au>
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#define LED_DISCO_RED_PORT GPIOB
#define LED_DISCO_RED_PIN GPIO2
#define LED_DISCO_GREEN_PORT GPIOE
#define LED_DISCO_GREEN_PIN GPIO8


int main(void)
{
	int i;
	int j = 0;
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOE);
	/* green led for ticking */
	gpio_mode_setup(LED_DISCO_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
			LED_DISCO_GREEN_PIN);
	gpio_mode_setup(LED_DISCO_RED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
			LED_DISCO_RED_PIN);

	while (1) {
		gpio_toggle(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);

		for (i = 0; i < 0x10000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
		gpio_toggle(LED_DISCO_RED_PORT, LED_DISCO_RED_PIN);
		for (i = 0; i < 0x10000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
	}

	return 0;
}
