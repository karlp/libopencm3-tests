/*
 * Aug 2016 Karl Palsson <karlp@tweak.net.au>
 */

#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/rcc.h>

/* Disco board is b6 */
#define LEDPORT GPIOB
#define LEDPIN GPIO6

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

int main(void)
{
	int i;
	int j = 0;
	/* Allow leds on any port */
	RCC_AHBENR |= 0xff;
	gpio_mode_setup(LEDPORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LEDPIN);
	/* blink twice with slow msi reset clock */
	hack_blink(4, 60);

	/* step forward to HSI/2, 8Mhz */
	struct rcc_clock_scale v2_8low = {
                .hpre = RCC_CFGR_HPRE_SYSCLK_DIV2,
                .ppre1 = RCC_CFGR_PPRE1_HCLK_NODIV,
                .ppre2 = RCC_CFGR_PPRE2_HCLK_NODIV,
                .voltage_scale = PWR_SCALE2,
                .flash_config = FLASH_ACR_LATENCY_0WS,
		.ahb_frequency = 8000000,
                .apb1_frequency = 8000000,
                .apb2_frequency = 8000000,
	};
	rcc_clock_setup_hsi(&v2_8low);
	/* blink twice again, different rate */
	hack_blink(4, 60);

	/* step forward to HSI->PLL@32Mhz, range 1 */
	rcc_clock_setup_pll(&rcc_clock_config[RCC_CLOCK_VRANGE1_HSI_PLL_32MHZ]);
	/* blink twice again */
	hack_blink(4, 400);

	/* back down again */
	rcc_clock_setup_hsi(&v2_8low);
	hack_blink(4, 60);

	/* just keep blinking */
	while (1) {
		hack_blink(1, 400);
	}

	return 0;
}
