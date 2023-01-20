/*
 * Karl Palsson <karlp@tweak.au> Jan 2023
 * hw file for libopencm3 "hw1" test board
 */

#include "hw.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>



/* 16 MHz external */
static const struct rcc_clock_scale clock_16m_hse = {
	.pll_source = RCC_CFGR_PLLSRC_HSE_CLK,
	.pll_mul = RCC_CFGR_PLLMUL_MUL6,
	.pll_div = RCC_CFGR_PLLDIV_DIV3,
	.hpre = RCC_CFGR_HPRE_SYSCLK_NODIV,
	.ppre1 = RCC_CFGR_PPRE1_HCLK_NODIV,
	.ppre2 = RCC_CFGR_PPRE2_HCLK_NODIV,
	.voltage_scale = PWR_SCALE1,
	.flash_waitstates = 1,
	.ahb_frequency = 32000000,
	.apb1_frequency = 32000000,
	.apb2_frequency = 32000000,
};


struct hw_detail hw_details = {
	.clock_config = &clock_16m_hse,
	.led_port = GPIOB,
	.led_pin = GPIO8,
	.periph = SPI2,
	.periph_rcc = RCC_SPI2,
	.periph_rst = RST_SPI2,
	.periph_nvic = NVIC_SPI2_IRQ,
	.periph_pins = GPIO13 | GPIO14 | GPIO15,
	.spi_cs = GPIO12,
	.periph_pins_afio = GPIO_AF5,
	.periph_port = GPIOB,
	.periph_port_rcc = RCC_GPIOB,
};
