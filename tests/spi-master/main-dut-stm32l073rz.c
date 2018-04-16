/*
 * March 2017 Karl Palsson <karlp@tweak.net.au>
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/usart.h>

#include "hw.h"

#define LED_DISCO_GREEN_PORT GPIOB  // actually cs
#define LED_DISCO_GREEN_PIN GPIO6


struct hw_detail hw_details = {
	.periph = SPI1,
	.periph_rcc = RCC_SPI1,
	.periph_rst = RST_SPI1,
	.pins = GPIO5| GPIO6 | GPIO7, /* SPI pins for setting AF with */
	.port = GPIOA,
	.port_rcc = RCC_GPIOA,
	.trigger_rcc = RCC_GPIOB,
	.trigger_port = GPIOB,
	.trigger_pin = GPIO8,   // pb6 is d7 on the LA
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
	gpio_set_output_options(hw_details.port, GPIO_OTYPE_PP, GPIO_OSPEED_MED, hw_details.pins);
	gpio_set_af(hw_details.port, GPIO_AF0, hw_details.pins);
}

static void test_init(void)
{
        /* Setup SPI parameters. */
	rcc_periph_clock_enable(hw_details.periph_rcc);
        spi_init_master(hw_details.periph, SPI_CR1_BAUDRATE_FPCLK_DIV_64, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
        /* Ignore the stupid NSS pin. */
        spi_enable_software_slave_management(hw_details.periph);
        spi_set_nss_high(hw_details.periph);

        /* Finally enable the SPI. */
        spi_enable(hw_details.periph);
}

static void test_task(void) {
	static int i = 0;
	gpio_set(hw_details.trigger_port, hw_details.trigger_pin);
	printf("Test iteration %d\n", i++);
	spi_xfer(hw_details.periph, 0xaa);
	spi_xfer(hw_details.periph, 0x42);
	spi_xfer(hw_details.periph, 0x69);

	gpio_clear(hw_details.trigger_port, hw_details.trigger_pin);
}

static
void setup_usart(void)
{
        uint32_t dev = USART2;
        rcc_periph_clock_enable(RCC_USART2);
        rcc_periph_clock_enable(RCC_GPIOA);
        gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2|GPIO3);
        gpio_set_af(GPIOA, GPIO_AF4, GPIO2|GPIO3);

        usart_set_baudrate(dev, 115200);
        usart_set_databits(dev, 8);
        usart_set_parity(dev, USART_PARITY_NONE);
        usart_set_stopbits(dev, USART_STOPBITS_1);
        usart_set_mode(dev, USART_MODE_TX_RX);
        usart_set_flow_control(dev, USART_FLOWCONTROL_NONE);

        /* Finally enable the USART. */
        usart_enable(dev);
}


static void setup(void)
{
	setup_usart();
	printf("hi guys!\n");
	hw_init();
	test_init();
}


int main(void)
{
	const struct rcc_clock_scale myclock = {
		.pll_source = RCC_CFGR_PLLSRC_HSI16_CLK,
		.pll_mul = RCC_CFGR_PLLMUL_MUL4,
		.pll_div = RCC_CFGR_PLLDIV_DIV2,
		.hpre = RCC_CFGR_HPRE_NODIV,
		.ppre1 = RCC_CFGR_PPRE1_NODIV,
		.ppre2 = RCC_CFGR_PPRE2_NODIV,
		.voltage_scale = PWR_SCALE1,
		.flash_waitstates = 1,
		.ahb_frequency = 32e6,
		.apb1_frequency = 32e6,
		.apb2_frequency = 32e6,
	};
	int i, j;
	rcc_clock_setup_pll(&myclock);
        rcc_periph_clock_enable(RCC_SYSCFG);

	/* green led for ticking */
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_mode_setup(LED_DISCO_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_DISCO_GREEN_PIN);
	gpio_set(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);
	setup();

	while (1) {
		test_task();
		//gpio_toggle(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);
		for (i = 0; i < 0x80000; i++) { /* Wait a bit. */
                        __asm__("NOP");
                }
	}
	return 0;
}
