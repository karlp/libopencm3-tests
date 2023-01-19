/*
 * Karl Palsson <karlp@etactica.com> Oct 2019
 * hw file for EM3 proto with stm32 host.
 * This is the the same as the "bar-faker1" board.
 */

#include "hw.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
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
	.product_code = 1234, // FIXME
	.led_port = GPIOB,
	.led_pin = GPIO8,
	.rs485de_port = GPIOA,
	.rs485de_pin = GPIO1,
	.mb_port = &hw_uart_mb_em3x,
};

void usart2_isr(void)
{
	//hw_mb_port_irq();
}
