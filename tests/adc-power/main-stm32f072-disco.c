/*
 * Copyright (C) 2015 Karl Palsson <karlp@tweak.net.au>
 */

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/crs.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include <stdio.h>
#include "adc-power.h"

static
void setup_usart(void)
{
	uint32_t dev = USART1;
	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO9);	
	
	usart_set_baudrate(dev, 115200);
	usart_set_databits(dev, 8);
	usart_set_parity(dev, USART_PARITY_NONE);
	usart_set_stopbits(dev, USART_CR2_STOPBITS_1);
	usart_set_mode(dev, USART_MODE_TX);
	usart_set_flow_control(dev, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(dev);
}

int main(void)
{
	int i;
	rcc_clock_setup_in_hsi48_out_48mhz();
	setup_usart();

	/* LED on for boot progress */
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);
	gpio_set(GPIOC, GPIO7);

	printf("hi guys!\n");

	rcc_periph_clock_enable(RCC_GPIOA);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO1);
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO5);
	adc_power_init();
	while (1) {
		adc_power_task_up();
		gpio_toggle(GPIOC, GPIO7);

		for (i = 0; i < 0x100000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
		printf("tick...\n");
		adc_power_task_down();
		gpio_toggle(GPIOC, GPIO7);
		for (i = 0; i < 0x100000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
	}

}

