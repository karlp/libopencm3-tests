/*
 * Aug 2016 Karl Palsson <karlp@tweak.net.au>
 */

#include <stdlib.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/rcc.h>

/* Disco board is b6 */
#define LEDPORT GPIOB
#define LEDPIN GPIO6

#ifndef ARRAY_LENGTH
#define ARRAY_LENGTH(array) (sizeof((array))/sizeof((array)[0]))
#endif

/*
 * A set of valid clock configurations with HSI as the source.
 * thankfully, L1 allows apb1/apb2 to reach full ahb speed in all cases.
 */
struct rcc_clock_scale valid_hsi_clocks[] = {
	{
		/* 8Mhz, hsi/div2, max perf for range 2 with 0 ws */
		.hpre = RCC_CFGR_HPRE_SYSCLK_DIV2,
		.ppre1 = RCC_CFGR_PPRE1_HCLK_NODIV,
		.ppre2 = RCC_CFGR_PPRE2_HCLK_NODIV,
		.voltage_scale = PWR_SCALE2,
		.flash_config = FLASH_ACR_LATENCY_0WS,
		.ahb_frequency = 8e6,
		.apb1_frequency = 8e6,
		.apb2_frequency = 8e6,
	},
	{
		/* Slowest hsi possible */
		.hpre = RCC_CFGR_HPRE_SYSCLK_DIV512,
		.ppre1 = RCC_CFGR_PPRE1_HCLK_NODIV,
		.ppre2 = RCC_CFGR_PPRE1_HCLK_NODIV,
		.voltage_scale = PWR_SCALE3,
		.flash_config = FLASH_ACR_LATENCY_0WS,
		.ahb_frequency = 31250,
		.apb1_frequency = 31250,
		.apb2_frequency = 31250,
	},
	{
		/* Fastest HSI possible, range 1, 0ws */
		.hpre = RCC_CFGR_HPRE_SYSCLK_NODIV,
		.ppre1 = RCC_CFGR_PPRE1_HCLK_NODIV,
		.ppre2 = RCC_CFGR_PPRE1_HCLK_NODIV,
		.voltage_scale = PWR_SCALE1,
		.flash_config = FLASH_ACR_LATENCY_0WS,
		.ahb_frequency = 16e6,
		.apb1_frequency = 16e6,
		.apb2_frequency = 16e6,
	},
	{
		/* highest perf for range 3 with 0 wait states */
		.hpre = RCC_CFGR_HPRE_SYSCLK_DIV8,
		.ppre1 = RCC_CFGR_PPRE1_HCLK_NODIV,
		.ppre2 = RCC_CFGR_PPRE1_HCLK_NODIV,
		.voltage_scale = PWR_SCALE3,
		.flash_config = FLASH_ACR_LATENCY_0WS,
		.ahb_frequency = 2e6,
		.apb1_frequency = 2e6,
		.apb2_frequency = 2e6,
	},
	{
		/* Fastest HSI for range 2, 0ws */
		.hpre = RCC_CFGR_HPRE_SYSCLK_NODIV,
		.ppre1 = RCC_CFGR_PPRE1_HCLK_NODIV,
		.ppre2 = RCC_CFGR_PPRE1_HCLK_NODIV,
		.voltage_scale = PWR_SCALE1,
		.flash_config = FLASH_ACR_LATENCY_0WS,
		.ahb_frequency = 16e6,
		.apb1_frequency = 16e6,
		.apb2_frequency = 16e6,
	},
	{
		/* highest perf for range 3 */
		.hpre = RCC_CFGR_HPRE_SYSCLK_DIV4,
		.ppre1 = RCC_CFGR_PPRE1_HCLK_NODIV,
		.ppre2 = RCC_CFGR_PPRE1_HCLK_NODIV,
		.voltage_scale = PWR_SCALE3,
		.flash_config = FLASH_ACR_LATENCY_1WS,
		.ahb_frequency = 4e6,
		.apb1_frequency = 4e6,
		.apb2_frequency = 4e6,
	},
};

int shuffled[ARRAY_LENGTH(valid_hsi_clocks)];

/**
 * blink led count times, with vile hack * 1000 asm nops
 */
static void hack_blink(int count, int hack)
{
	for (int i = 0; i < count; i++) {
		gpio_toggle(LEDPORT, LEDPIN);
		for (int k = 0; k < hack * 1000; k++) {
			__asm__("nop");
		}
	}
}

/* Stack overflow, wiki, whathaveyou */
/* FIXME oh yeah, did I mention that rand() is always zero? */
static void shuffle_fisher_yates(int *array, size_t n)
{
	if (n > 1) {
		size_t i;
		for (i = 0; i < n - 1; i++) {
			size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
			int t = array[j];
			array[j] = array[i];
			array[i] = t;
		}
	}
}

static void run_clock_set(void)
{
	shuffle_fisher_yates(shuffled, ARRAY_LENGTH(shuffled));

	struct rcc_clock_scale *clock;
	for (size_t i = 0; i < ARRAY_LENGTH(shuffled); i++) {
		clock = &valid_hsi_clocks[i];
		rcc_clock_setup_hsi(clock);
		/** blink quickly/slowly as we run through these */
		hack_blink(10, clock->ahb_frequency / (500 * 1000));
		hack_blink(4, clock->ahb_frequency / (200 * 1000));
	}
}

int main(void)
{
	/* Allow leds on any port */
	RCC_AHBENR |= 0xff;
	gpio_mode_setup(LEDPORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LEDPIN);
	/* blink twice with slow msi reset clock */
	hack_blink(4, 60);
	
	/* ok, now, "randomly" sort the list of clocks to try them out */
	for (size_t i = 0; i < ARRAY_LENGTH(shuffled); i++) {
		shuffled[i] = i;
	}

	while (1) {
		run_clock_set();
	}

	return 0;
}
