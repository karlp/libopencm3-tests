/*
 * Oct 2015 Karl Palsson <karlp@tweak.net.au>
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#include "trace.h"
#include "adc-power.h"

/* f3 pll setup, based on l1/f4*/

typedef struct {
	uint8_t pll_mul;
	uint8_t pll_div;
	uint8_t pll_source;
	uint32_t flash_config;
	uint8_t hpre;
	uint8_t ppre1;
	uint8_t ppre2;
	uint32_t apb1_frequency;
	uint32_t apb2_frequency;
	uint32_t ahb_frequency;
} rcc_clock_scale_t;

static void rcc_clock_setup_pll_f3_special(const rcc_clock_scale_t *clock)
{
	/* Turn on the appropriate source for the PLL */
	// TODO, some f3's have extra bits here
	enum rcc_osc my_osc;
	if (clock->pll_source == RCC_CFGR_PLLSRC_HSE_PREDIV) {
		my_osc = RCC_HSE;
	} else {
		my_osc = RCC_HSI;
	}
	rcc_osc_on(my_osc);
	while (!rcc_is_osc_ready(my_osc));

	/* Configure flash settings. */
	flash_set_ws(clock->flash_config);

	/*
	 * Set prescalers for AHB, ADC, ABP1, ABP2.
	 * Do this before touching the PLL (TODO: why?).
	 */
	rcc_set_hpre(clock->hpre);
	rcc_set_ppre1(clock->ppre1);
	rcc_set_ppre2(clock->ppre2);

	rcc_osc_off(RCC_PLL);
	while (rcc_is_osc_ready(RCC_PLL));
	rcc_set_pll_source(clock->pll_source);
	rcc_set_pll_multiplier(clock->pll_mul);
	// TODO - iff pll_div != 0, then maybe we're on a target that 
	// has the dividers?

	/* Enable PLL oscillator and wait for it to stabilize. */
	rcc_osc_on(RCC_PLL);
	while (!rcc_is_osc_ready(RCC_PLL));

	/* Select PLL as SYSCLK source. */
	rcc_set_sysclk_source(RCC_CFGR_SW_PLL);
	rcc_wait_for_sysclk_status(RCC_PLL);

	/* Set the peripheral clock frequencies used. */
	rcc_ahb_frequency = clock->ahb_frequency;
	rcc_apb1_frequency = clock->apb1_frequency;
	rcc_apb2_frequency = clock->apb2_frequency;
}

static void setup_clocks(void)
{
	rcc_clock_scale_t clock_full_hse8mhz ={
		.pll_mul = RCC_CFGR_PLLMUL_PLL_IN_CLK_X9,
		.pll_source = RCC_CFGR_PLLSRC_HSE_PREDIV,
		.hpre = RCC_CFGR_HPRE_DIV_NONE,
		.ppre1 = RCC_CFGR_PPRE1_DIV_2,
		.ppre2 = RCC_CFGR_PPRE2_DIV_NONE,
		.flash_config = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2WS,
		.apb1_frequency = 36000000,
		.apb2_frequency = 72000000,
		.ahb_frequency = 72000000,
	};

	rcc_clock_setup_pll_f3_special(&clock_full_hse8mhz);
}

int main(void)
{
	int i;
	int j = 0;
	setup_clocks();
	/* Board led */
	rcc_periph_clock_enable(RCC_GPIOE);
	gpio_mode_setup(GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8);
	gpio_set(GPIOE, GPIO8);
	printf("hi guys!\n");

	// ADC seems to take these no matter what?
//	rcc_periph_clock_enable(RCC_GPIOA);
//	rcc_periph_clock_enable(RCC_GPIOF);
//	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO0);
//	gpio_mode_setup(GPIOF, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO4);

	adc_power_init();
	for (i = 0; i < 0x1000; i++) { /* need as much as 10 usecs for vreg */
		__asm__("NOP");
	}
	while (1) {
		adc_power_task_up();
		gpio_toggle(GPIOE, GPIO8);

		for (i = 0; i < 0x100000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
		printf("tick...\n");
		adc_power_task_down();
		gpio_toggle(GPIOE, GPIO8);
		for (i = 0; i < 0x100000; i++) { /* Wait a bit. */
			__asm__("NOP");
		}
	}

	return 0;
}
