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

#define LED_DISCO_GREEN_PORT GPIOD
#define LED_DISCO_GREEN_PIN GPIO12

#define CODEC_ADDRESS 0x4a


static void codec_gpio_init(void)
{
	/* reset pin */
	rcc_periph_clock_enable(RCC_GPIOD);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);

	/* i2c control lines */
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6 | GPIO9);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO6 | GPIO9);
	gpio_set_af(GPIOB, GPIO_AF4, GPIO6 | GPIO9);
}

static void codec_i2c_init(void)
{
	rcc_periph_clock_enable(RCC_I2C1);
	i2c_peripheral_disable(I2C1);
	i2c_reset(I2C1);
	i2c_set_standard_mode(I2C1);
	i2c_enable_ack(I2C1);
	i2c_set_dutycycle(I2C1, I2C_CCR_DUTY_DIV2); /* default, no need to do this really */
	i2c_set_clock_frequency(I2C1, I2C_CR2_FREQ_42MHZ);
	/* 42MHz / (100kHz * 2) */
	i2c_set_ccr(I2C1, 210);
	/* standard mode, freqMhz+1*/
	i2c_set_trise(I2C1, 43);
	i2c_peripheral_enable(I2C1);
}

static void codec_init(void)
{
	int i;
	/* Configure the Codec related IOs */
	codec_gpio_init();

	/* reset the codec */
	gpio_clear(GPIOD, GPIO4);
	for (i = 0; i < 1000000; i++) { /* Wait a bit. */
		__asm__("NOP");
	}
	gpio_set(GPIOD, GPIO4);

	codec_i2c_init();
}

static int codec_write_reg(uint8_t reg, uint8_t val)
{
	uint32_t i2c = I2C1;

	while ((I2C_SR2(i2c) & I2C_SR2_BUSY)) {
	}

	i2c_send_start(i2c);

	/* Wait for master mode selected */
	while (!((I2C_SR1(i2c) & I2C_SR1_SB)
		& (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

	i2c_send_7bit_address(i2c, CODEC_ADDRESS, I2C_WRITE);

	/* Waiting for address is transferred. */
	while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));

	/* Cleaning ADDR condition sequence. */
	uint32_t reg32 = I2C_SR2(i2c);
	(void) reg32; /* unused */

	/* Common above here */

	/* Sending the data. */
	i2c_send_data(i2c, reg);
	while (!(I2C_SR1(i2c) & (I2C_SR1_BTF)));
	i2c_send_data(i2c, val);
	while (!(I2C_SR1(i2c) & (I2C_SR1_BTF | I2C_SR1_TxE)));

	/* Send STOP condition. */
	i2c_send_stop(i2c);
	return 0;
}

static uint32_t codec_read_reg(uint8_t reg)
{
	uint32_t i2c = I2C1;

	while ((I2C_SR2(i2c) & I2C_SR2_BUSY)) {
	}

	i2c_send_start(i2c);

	/* Wait for master mode selected */
	while (!((I2C_SR1(i2c) & I2C_SR1_SB)
		& (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

	i2c_send_7bit_address(i2c, CODEC_ADDRESS, I2C_WRITE);

	/* Waiting for address is transferred. */
	while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));

	/* Cleaning ADDR condition sequence. */
	uint32_t reg32 = I2C_SR2(i2c);
	(void) reg32; /* unused */

	/*  Common stuff ABOVE HERE     */

	i2c_send_data(i2c, reg);
	while (!(I2C_SR1(i2c) & (I2C_SR1_BTF)));

	i2c_send_start(i2c);

	/* Wait for master mode selected */
	while (!((I2C_SR1(i2c) & I2C_SR1_SB)
		& (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

	i2c_send_7bit_address(i2c, CODEC_ADDRESS, I2C_READ);

	/* Waiting for address is transferred. */
	while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));

	i2c_disable_ack(i2c);

	/* Cleaning ADDR condition sequence. */
	reg32 = I2C_SR2(i2c);
	(void) reg32; /* unused */

	i2c_send_stop(i2c);

	while (!(I2C_SR1(i2c) & I2C_SR1_RxNE));
	uint32_t result = i2c_get_data(i2c);

	i2c_enable_ack(i2c);
	I2C_SR1(i2c) &= ~I2C_SR1_AF;
	return result;
}

static void codec_readid(void)
{
	uint8_t res = codec_read_reg(0x01);
	printf("raw res = %#x Codec is %#x (should be 0x1c), revision %d\n", res, res >> 3, res & 0x7);
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
	codec_readid();

	codec_write_reg(0x14, 0xff);
	for (i = 0; i < 8; i++) {
		uint8_t pass_vol_a = codec_read_reg(0x14);
		printf("Passthrough vol A was: %#x\n", pass_vol_a);
		codec_write_reg(0x14, pass_vol_a >> 1);
		gpio_toggle(LED_DISCO_GREEN_PORT, LED_DISCO_GREEN_PIN);
		for (j = 0; j < 100000; j++) { /* Wait a bit. */
			__asm__("NOP");
		}
	}

	/* Nothing else to do */;
	while (1) {
		;
	}
	return 0;
}
