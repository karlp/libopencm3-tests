/*
 * A port of the libopencm3 gadget-zero tests to run under freertos.
 * "just because"
 */

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/syscfg.h>

#include <stdio.h>

#include <cmsis_os2.h>

#include "usb-gadget0.h"

#define ER_DEBUG
#ifdef ER_DEBUG
#define ER_DPRINTF(fmt, ...) \
	do { printf(fmt, ## __VA_ARGS__); } while (0)
#else
#define ER_DPRINTF(fmt, ...) \
	do { } while (0)
#endif

const struct rcc_clock_scale this_clock_config = {
		/* 32MHz PLL from 16MHz HSE, 96MHz for USB on PLL VCO out */
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

static void taskUSBD(void *args)
{
	usbd_device *usbd_dev = args;
        int i = 0;
        while (1) {
		gpio_set(GPIOB, GPIO9);
		gadget0_run(usbd_dev);
		gpio_clear(GPIOB, GPIO9);
	}
}

int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8|GPIO9);
	gpio_set(GPIOB, GPIO8);
	rcc_clock_setup_pll(&this_clock_config);

	/* Enable built in USB pullup on L1 */
	rcc_periph_clock_enable(RCC_SYSCFG);
	SYSCFG_PMC |= SYSCFG_PMC_USB_PU;

	usbd_device *usbd_dev = gadget0_init(&st_usbfs_v1_usb_driver,
					     "rtos2-stm32l1-hw1");

	ER_DPRINTF("bootup complete\n");
	gpio_clear(GPIOB, GPIO8);

	osKernelInitialize();
	osThreadAttr_t attrUSBD = {
		.name = "usbd"
	};
	osThreadNew(taskUSBD, usbd_dev, &attrUSBD);
        osKernelStart();

	return 0;
}

