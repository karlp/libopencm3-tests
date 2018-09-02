/*
 * usb to spi master bridge, using freertos
 * Consider to be BSD2 Clause, Apache 2.0, MIT, or ISC licensed, at your
 * pleasure.
 * karl Palsson <karlp@tweak.net.au>
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>

#include "hw.h"
#include "trace.h"

struct hw_detail hw_details = {
	.periph = SPI2,
	.periph_rcc = RCC_SPI2,
	.periph_rst = RST_SPI2,
	.pins = GPIO13 | GPIO14 | GPIO15, /* SPI pins for setting AF with */
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


static void prvTaskGreenBlink1(void *pvParameters)
{
	(void) pvParameters;
	int i = 0;
	while (1) {
		printf("gblink %d\n", i++);
		vTaskDelay(portTICK_PERIOD_MS * 550);
		gpio_toggle(hw_details.trigger_port, hw_details.trigger_pin);
	}

	/* Tasks must not attempt to return from their implementing
	function or otherwise exit.  In newer FreeRTOS port
	attempting to do so will result in an configASSERT() being
	called if it is defined.  If it is necessary for a task to
	exit then have the task call vTaskDelete( NULL ) to ensure
	its exit is clean. */
	vTaskDelete(NULL);
}

static void prvTaskSpiMaster(void *pvParameters)
{
	(void)pvParameters;
        rcc_periph_clock_enable(hw_details.periph_rcc);
        spi_init_master(hw_details.periph, SPI_CR1_BAUDRATE_FPCLK_DIV_32, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
        /* Ignore the stupid NSS pin. */
        spi_enable_software_slave_management(hw_details.periph);
        spi_set_nss_high(hw_details.periph);

        /* Finally enable the SPI. */
        spi_enable(hw_details.periph);

	int i = 0;
	while (1) {
		printf("spi send %d\n", i);
		vTaskDelay(portTICK_PERIOD_MS * 200);
	        spi_xfer(hw_details.periph, 0xaa);
	        spi_xfer(hw_details.periph, i++);
	}

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
	hw_init();
	printf("starting freertos...\n");

	xTaskCreate(prvTaskGreenBlink1, "green.blink", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(prvTaskSpiMaster, "spi.master", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

	vTaskStartScheduler();

	return 0;
}

void vAssertCalled(const char * const pcFileName, unsigned long ulLine)
{
	volatile unsigned long ulSetToNonZeroInDebuggerToContinue = 0;

	/* Parameters are not used. */
	(void) ulLine;
	(void) pcFileName;

	taskENTER_CRITICAL();
	{
		while (ulSetToNonZeroInDebuggerToContinue == 0) {
			/* Use the debugger to set ulSetToNonZeroInDebuggerToContinue to a
			non zero value to step out of this function to the point that raised
			this assert(). */
			__asm volatile( "NOP");
			__asm volatile( "NOP");
		}
	}
	taskEXIT_CRITICAL();
}

