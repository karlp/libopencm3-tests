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

struct hw_detail hw_details = {
	.periph = SPI2,
	.periph_rcc = RCC_SPI2,
	.periph_rst = RST_SPI2,
	.pins = GPIO13| GPIO14 | GPIO15, /* SPI pins for setting AF with */
	.port = GPIOB,
	.port_rcc = RCC_GPIOB,
	.trigger_rcc = RCC_GPIOB,
	.trigger_port = GPIOB,
	.trigger_pin = GPIO9,
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
	/* mostly, this is just "write 0 to cr1" */
	spi_set_slave_mode(hw_details.periph);
	spi_send_msb_first(hw_details.periph);
	spi_set_dff_8bit(hw_details.periph);
	spi_set_clock_phase_0(hw_details.periph);
	spi_set_clock_polarity_0(hw_details.periph);
	spi_set_frf_motorola(hw_details.periph);

	/* we're a spi slave, use a CS pin */
	spi_disable_software_slave_management(hw_details.periph);
	SPI_CR2(hw_details.periph) &= ~SPI_CR2_SSOE;

        /* Finally enable the SPI. */
        spi_enable(hw_details.periph);
}

static void test_task(void) {
	static int i = 0;
	uint32_t spi = hw_details.periph;
	if (SPI_SR(spi) & SPI_SR_TXE) {
		/* ready to load next data in */
		SPI_DR(spi) = i++;
	}

	if (SPI_SR(spi) & SPI_SR_RXNE) {
		uint8_t data = SPI_DR(spi);
		trace_send8(2, data);
	}
}


static void setup(void)
{
	printf("hi guys!\n");
	hw_init();
	test_init();
}


int main(void)
{
	const struct rcc_clock_scale myclock = {
		.pll_source = RCC_CFGR_PLLSRC_HSE_CLK,
		.pll_mul = RCC_CFGR_PLLMUL_MUL4,
		.pll_div = RCC_CFGR_PLLDIV_DIV2,
		.hpre = RCC_CFGR_HPRE_SYSCLK_NODIV,
		.ppre1 = RCC_CFGR_PPRE1_HCLK_NODIV,
		.ppre2 = RCC_CFGR_PPRE2_HCLK_NODIV,
		.voltage_scale = PWR_SCALE1,
		.flash_waitstates = 1,
		.ahb_frequency = 32e6,
		.apb1_frequency = 32e6,
		.apb2_frequency = 32e6,
	};
	int i, j;
	rcc_clock_setup_pll(&myclock);
	setup();

	while (1) {
		test_task();
	}
	return 0;
}
