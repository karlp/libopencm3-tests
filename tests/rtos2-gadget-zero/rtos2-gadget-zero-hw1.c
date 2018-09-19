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
#include "trace.h"

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

osEventFlagsId_t evt_usbd;
osMessageQueueId_t mq_usbd;
osThreadId_t threadUSBD;

static void taskUSBD(void *args)
{
	(void)args;
	/* Enable built in USB pullup on L1 */
	rcc_periph_clock_enable(RCC_SYSCFG);
	SYSCFG_PMC |= SYSCFG_PMC_USB_PU;

	//evt_usbd = osEventFlagsNew(NULL);
	mq_usbd = osMessageQueueNew(32, 1, NULL);
	usbd_device *usbd_dev = gadget0_init(&st_usbfs_v1_usb_driver,
					     "rtos2-stm32l1-hw1");
	//nvic_set_priority(NVIC_USB_LP_IRQ, 6<<4);
	nvic_enable_irq(NVIC_USB_LP_IRQ);

	ER_DPRINTF("USBD: loop start\n");
	while (1) {
		//uint32_t flags = osEventFlagsWait(evt_usbd, 1, osFlagsWaitAny, osWaitForever);
		//trace_send8(1, flags);
		char x;
		osStatus_t stat = osMessageQueueGet(mq_usbd, &x, NULL, 500);
		if (stat == osOK) {
			gpio_set(GPIOB, GPIO9);
			gadget0_run(usbd_dev);
			gpio_clear(GPIOB, GPIO9);
			nvic_enable_irq(NVIC_USB_LP_IRQ);
		} else {
			ER_DPRINTF(".");
		}
		//ER_DPRINTF("sl %lu\n", osThreadGetStackSpace(threadUSBD));
	}
}

int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8|GPIO9);
	gpio_set(GPIOB, GPIO8);
	rcc_clock_setup_pll(&this_clock_config);

	ER_DPRINTF("bootup complete\n");
	gpio_clear(GPIOB, GPIO8);

	osKernelInitialize();
	osThreadAttr_t attrUSBD = {
		.name = "usbd",
		.priority = osPriorityAboveNormal,
		.stack_size = 1024,
	};
	threadUSBD = osThreadNew(taskUSBD, NULL, &attrUSBD);
	ER_DPRINTF("stacksize = %lu\n", osThreadGetStackSize(threadUSBD));
        osKernelStart();

	return 0;
}

void usb_lp_isr(void)
{
	nvic_disable_irq(NVIC_USB_LP_IRQ);
#if 0
	int x = osEventFlagsSet(evt_usbd, 1);
	trace_send8(2, '!');
	if (x < 0) {
		ER_DPRINTF("Failed to set flag: %d\n", x);
	}
#endif
	char dummy = 1;
	osMessageQueuePut(mq_usbd, &dummy, 0, 0);
}
