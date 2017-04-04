/*
 * Copyright (C) 2017 Karl Palsson <karlp@tweak.net.au>
 */

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/crs.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include <stdio.h>
#include "hw.h"
#include "i2c-master.h"

#define LED_DISCO_GREEN_PORT GPIOC
#define LED_DISCO_GREEN_PIN GPIO9



struct hw_detail hw_details = {
	.periph = I2C1,
	.periph_rcc = RCC_I2C1,
	.periph_rst = RST_I2C1,
	.pins = GPIO8 | GPIO9, /* our external i2c device on I2c1 */
	.port = GPIOB,
	.port_rcc = RCC_GPIOB,
	.trigger_rcc = RCC_GPIOB,
	.trigger_port = GPIOB,
	.trigger_pin = GPIO12,
	.i2c_clock_megahz = 48,
};


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
	usart_set_stopbits(dev, USART_CR2_STOP_1_0BIT);
	usart_set_mode(dev, USART_MODE_TX);
	usart_set_flow_control(dev, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(dev);
}

/**
 * Setup any gpios or anything hardware specific.
 * Should _only_ be things that can't be done in shared i2cm_init!
 */
static void i2cm_hw_init(void)
{
	/* trigger pin gpio */
	rcc_periph_clock_enable(hw_details.trigger_rcc);
	gpio_mode_setup(hw_details.trigger_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, hw_details.trigger_pin);

	/* i2c control lines */
	rcc_periph_clock_enable(hw_details.port_rcc);
	gpio_mode_setup(hw_details.port, GPIO_MODE_AF, GPIO_PUPD_NONE, hw_details.pins);
	gpio_set_output_options(hw_details.port, GPIO_OTYPE_OD, GPIO_OSPEED_HIGH, hw_details.pins);
	gpio_set_af(hw_details.port, GPIO_AF1, hw_details.pins);

	/* select sysclk as i2c clock! */
	RCC_CFGR3 |= RCC_CFGR3_I2C1SW;
}


static void setup(void)
{
	setup_usart();
	printf("hi guys!\n");
	i2cm_hw_init();
	i2cm_init();
}

int main(void)
{
	int i;
	rcc_clock_setup_in_hsi48_out_48mhz();
	/* green led for ticking */
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_mode_setup(LED_DISCO_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_DISCO_GREEN_PIN);
	gpio_set(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);

	setup();

	while (1) {
		i2cm_task();
		gpio_toggle(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);
		for (i = 0; i < 0x800000; i++) { /* Wait a bit. */
                        __asm__("NOP");
                }
	}

}

