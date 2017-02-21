/*
 * Feb 2017 Karl Palsson <karlp@tweak.net.au>
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/rcc.h>

#include "trace.h"

#include "hw.h"
#include "i2c-master.h"

#define LED_DISCO_GREEN_PORT GPIOD
#define LED_DISCO_GREEN_PIN GPIO12


struct hw_detail hw_details = {
	.periph = I2C1,
	.periph_rcc = RCC_I2C1,
	.periph_rst = RST_I2C1,
//	.pins = GPIO6 | GPIO9, /* FIXME - only for onboard! */
	.pins = GPIO8 | GPIO9, /* For SHT21 on I2c1 */
	.port = GPIOB,
	.port_rcc = RCC_GPIOB,
};


static void setup_i2c_gpio(void)
{
	/* reset pin */
//	rcc_periph_clock_enable(RCC_GPIOD);
//	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);

	/* i2c control lines */
	rcc_periph_clock_enable(hw_details.port_rcc);
	gpio_mode_setup(hw_details.port, GPIO_MODE_AF, GPIO_PUPD_NONE, hw_details.pins);
	gpio_set_output_options(hw_details.port, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, hw_details.pins);
	gpio_set_af(hw_details.port, GPIO_AF4, hw_details.pins);
}

static void codec_init(void)
{
	int i;
	/* Configure the Codec related IOs */
	setup_i2c_gpio();

	/* reset the codec */
//	gpio_clear(GPIOD, GPIO4);
	for (i = 0; i < 1000000; i++) { /* Wait a bit. */
		__asm__("NOP");
	}
//	gpio_set(GPIOD, GPIO4);

	i2cm_init();
}


int main(void)
{
	int i, j;
	rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
	/* green led for ticking */
	rcc_periph_clock_enable(RCC_GPIOD);
	gpio_mode_setup(LED_DISCO_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
		LED_DISCO_GREEN_PIN);
	printf("hi guys!\n");
	codec_init();

	while (1) {
		i2cm_task();
		gpio_toggle(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);
		for (i = 0; i < 0x800000; i++) { /* Wait a bit. */
                        __asm__("NOP");
                }
	}
	return 0;
}
