/*
 * March 2017 Karl Palsson <karlp@tweak.net.au>
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>

#include "trace.h"

#include "hw.h"
//#include "i2c-master.h"

#define LED_DISCO_GREEN_PORT GPIOB
#define LED_DISCO_GREEN_PIN GPIO7


struct hw_detail hw_details = {
	.periph = SPI2,
	.periph_rcc = RCC_SPI2,
	.periph_rst = RST_SPI2,
	.pins = GPIO13| GPIO14 | GPIO15, /* SPI pins for setting AF with */
	.port = GPIOB,
	.port_rcc = RCC_GPIOB,
	.trigger_rcc = RCC_GPIOB,
	.trigger_port = GPIOB,
	.trigger_pin = GPIO11,
};


/* provided in board files please*/
/**
 * Setup any gpios or anything hardware specific.
 * Should _only_ be things that can't be done in shared init()
 */
static void hw_init(void)
{
	/* trigger pin gpio */
	rcc_periph_clock_enable(hw_details.trigger_rcc);
	gpio_mode_setup(hw_details.trigger_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, hw_details.trigger_pin);

	/* spi control lines */
	rcc_periph_clock_enable(hw_details.port_rcc);
	gpio_mode_setup(hw_details.port, GPIO_MODE_AF, GPIO_PUPD_NONE, hw_details.pins);
	gpio_set_output_options(hw_details.port, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, hw_details.pins);
	gpio_set_af(hw_details.port, GPIO_AF5, hw_details.pins);
}

static void test_init(void)
{
        /* Setup SPI parameters. */
	rcc_periph_clock_enable(hw_details.periph_rcc);
        spi_init_master(hw_details.periph, SPI_CR1_BAUDRATE_FPCLK_DIV_16, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
        /* Ignore the stupid NSS pin. */
        spi_enable_software_slave_management(hw_details.periph);
        //spi_enable_ss_output(MRF_SPI);
        spi_set_nss_high(hw_details.periph);
        /* Finally enable the SPI. */
        spi_enable(hw_details.periph);
}

static void test_task(void) {
	static int i = 0;
	printf("Test iteration %d\n", i++);
	gpio_set(hw_details.trigger_port, hw_details.trigger_pin);
	spi_xfer(hw_details.periph, 0xaa);
	spi_xfer(hw_details.periph, 0x42);
	spi_xfer(hw_details.periph, 0x69);
	gpio_clear(hw_details.trigger_port, hw_details.trigger_pin);
}

static void setup(void)
{
	printf("hi guys!\n");
	hw_init();
	test_init();
}


int main(void)
{
	int i, j;
	rcc_clock_setup_pll(&rcc_clock_config[RCC_CLOCK_VRANGE1_HSI_PLL_32MHZ]);
	/* green led for ticking */
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_mode_setup(LED_DISCO_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
		LED_DISCO_GREEN_PIN);
	setup();

	while (1) {
		test_task();
		gpio_toggle(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);
		for (i = 0; i < 0x800000; i++) { /* Wait a bit. */
                        __asm__("NOP");
                }
	}
	return 0;
}
