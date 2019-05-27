/*
 * spi slave test unit, using freertos
 * Consider to be BSD2 Clause, Apache 2.0, MIT, or ISC licensed, at your
 * pleasure.
 * karl Palsson <karlp@tweak.net.au>
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/syscfg.h>

#include "hw.h"
#include "trace.h"
#include "queue.h"

// no trace on cm0
#define ER_DEBUG
#ifdef ER_DEBUG
#define ER_DPRINTF(fmt, ...) \
	do { printf(fmt, ## __VA_ARGS__); } while (0)
#else
#define ER_DPRINTF(fmt, ...) \
	do { } while (0)
#endif

#ifndef ARRAY_LENGTH
#define ARRAY_LENGTH(array) (sizeof((array))/sizeof((array)[0]))
#endif

/* Same for l0 as l1 nucleo */
struct hw_detail hw_details = {
	.periph = SPI1,
	.periph_rcc = RCC_SPI1,
	.periph_rst = RST_SPI1,
	.periph_irq = NVIC_SPI1_IRQ,
	.pins = GPIO5 | GPIO6 | GPIO7, /* SPI pins for setting AF with */
	.port = GPIOA,
	.port_rcc = RCC_GPIOA,
	/* Might be software, might be hardware controlled */
	.cs_rcc = RCC_GPIOB,
	.cs_port = GPIOB,
	.cs_pin = GPIO6,
};

QueueHandle_t spiQ_rx;
QueueHandle_t spiQ_tx;

/* provided in board files please*/
/**
 * Setup any gpios or anything hardware specific.
 * Should _only_ be things that can't be done in shared init()
 */
static void hw_init(void)
{
	/* trigger pin gpio */
	rcc_periph_clock_enable(hw_details.trigger_rcc);
	/* "spare" "led" line */
	gpio_mode_setup(hw_details.led_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, hw_details.led_pin);

	/* spi control lines */
	rcc_periph_clock_enable(hw_details.port_rcc);
	gpio_mode_setup(hw_details.port, GPIO_MODE_AF, GPIO_PUPD_PULLUP, hw_details.pins);
//	gpio_set_output_options(hw_details.port, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, hw_details.pins);
	// For L0 gpio_set_af(hw_details.port, GPIO_AF0, hw_details.pins);
	gpio_set_af(hw_details.port, GPIO_AF5, hw_details.pins);

	rcc_periph_clock_enable(hw_details.cs_rcc);
	gpio_mode_setup(hw_details.cs_port, GPIO_MODE_AF, GPIO_PUPD_PULLUP, hw_details.cs_pin);
//	gpio_set_output_options(hw_details.port, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, hw_details.pins);
	// WHAT? gpio_set_af(hw_details.cs_port, GPIO_AF5, hw_details.cs_pins);

}


static void prvTaskGreenBlink1(void *pvParameters)
{
	(void) pvParameters;
	int i = 0;
	while (1) {
		printf("gblink %d\n", i++);
		vTaskDelay(portTICK_PERIOD_MS * 500);
		//gpio_toggle(hw_details.led_port, hw_details.led_pin);
	}

	/* Tasks must not attempt to return from their implementing
	function or otherwise exit.  In newer FreeRTOS port
	attempting to do so will result in an configASSERT() being
	called if it is defined.  If it is necessary for a task to
	exit then have the task call vTaskDelete( NULL ) to ensure
	its exit is clean. */
	vTaskDelete(NULL);
}

bool exti_direction_falling;
void exti9_5_isr(void)
{
	// Turn on spi when it goes low.
	exti_reset_request(EXTI6);

        if (exti_direction_falling) {
		spi_enable(hw_details.periph);
                exti_direction_falling = false;
                exti_set_trigger(EXTI6, EXTI_TRIGGER_RISING);
        } else {
		// TODO - if it goes high again, make sure we reset our state machine!
		spi_disable(hw_details.periph);
                exti_direction_falling = true;
                exti_set_trigger(EXTI6, EXTI_TRIGGER_FALLING);
        }
}

void spi1_isr(void)  {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	uint32_t flags = SPI_SR(hw_details.periph);
	trace_send_blocking16(3, flags);
	if (flags & SPI_SR_RXNE) {
		//spi_disable_rx_buffer_not_empty_interrupt(hw_details.periph);
		uint8_t x = spi_read(hw_details.periph);
		trace_send_blocking8(4, x);
		xQueueSendFromISR(spiQ_rx, &x, &xHigherPriorityTaskWoken);
	}
	if (flags & SPI_SR_TXE) {
		uint8_t x;
		BaseType_t rc = xQueueReceiveFromISR(spiQ_tx, &x, &xHigherPriorityTaskWoken);
		if (rc == pdFALSE) {
			//x = 0xfa; // default data to reply with
			x = 0;
		}
		spi_write(hw_details.periph, x);
		trace_send_blocking8(5, x);
	}

	if (xHigherPriorityTaskWoken) {
		//taskYIELD_FROM_ISR(); /* documented freertos name */
		// and it's fucking borked. wat
		//portYIELD_FROM_ISR(); /* actual name, thanks freertos */
	}
}

/* easier to debug here */
uint8_t spi_regs[] = { 0xaa, 0xca, 0xfe, 0xbe, 0xef };
static void prvTaskSpiSlave(void *pvParameters)
{
	(void)pvParameters;

	/* NOTE: queues not stream/message buffers because freertos 10.x is busted in openocd, no other reason */
	spiQ_rx = xQueueCreate(16, 1);
	configASSERT(spiQ_rx);
	spiQ_tx = xQueueCreate(16, 1);
	configASSERT(spiQ_tx);

        rcc_periph_clock_enable(hw_details.periph_rcc);

	spi_set_slave_mode(hw_details.periph);
	// PB6 is not the real NSS pin, so have to do it... magically.
	spi_enable_software_slave_management(hw_details.periph);
	//spi_enable(hw_details.periph);

	rcc_periph_clock_enable(hw_details.cs_rcc);
	rcc_periph_clock_enable(RCC_SYSCFG);
	gpio_mode_setup(hw_details.cs_port, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, hw_details.cs_pin);
	exti_select_source(EXTI6, hw_details.cs_port);
        exti_set_trigger(EXTI6, EXTI_TRIGGER_FALLING);
	exti_direction_falling = true;
        exti_enable_request(EXTI6);
        nvic_set_priority(NVIC_EXTI9_5_IRQ, 6<<4);
	nvic_enable_irq(NVIC_EXTI9_5_IRQ);

	spi_enable_rx_buffer_not_empty_interrupt(hw_details.periph);
	spi_enable_tx_buffer_empty_interrupt(hw_details.periph);
	/* numerically greater than free rtos kernel split (lower priority) */
        nvic_set_priority(hw_details.periph_irq, 6<<4);
	nvic_enable_irq(hw_details.periph_irq);

	/* Prestuff first byte */
	SPI_DR(hw_details.periph) = 0;

	while(1) {
		/* Block on a receive fifo, statemachine on that... */
		uint8_t in, out;
		BaseType_t rc = xQueueReceive(spiQ_rx, &in, portMAX_DELAY);
		configASSERT(rc); /* We want to block forever */
		trace_send_blocking8(2, in);
		bool write = in & 0x80;
		unsigned reg = in & 0x7f;
		if (reg > ARRAY_LENGTH(spi_regs)) {
			/* how do we flag errors? */
			ER_DPRINTF("out of bounds register requested");
		}
		/* always need to consume, in write _and_ read mode! */
		rc = xQueueReceive(spiQ_rx, &in, portMAX_DELAY);
		configASSERT(rc);
		if (write) {
			trace_send_blocking8(2, in);
			spi_regs[reg] = in;
		} else {
			out = spi_regs[reg];
			xQueueSend(spiQ_tx, &out, portMAX_DELAY);
			trace_send_blocking8(6, out);
		}
	}

}


int main(void)
{
#if 0 /* l0 */
        const struct rcc_clock_scale myclock = {
                .ahb_frequency = 32e6,
                .apb1_frequency = 32e6,
                .apb2_frequency = 32e6,
                .flash_waitstates = 1,
                .pll_source = RCC_CFGR_PLLSRC_HSI16_CLK, /* not even sure there's hse on l053 disco */
                /* .msi_range  doesn't matter */
                .pll_mul = RCC_CFGR_PLLMUL_MUL4,
                .pll_div = RCC_CFGR_PLLDIV_DIV2,
                .hpre = RCC_CFGR_HPRE_NODIV,
                .ppre1 = RCC_CFGR_PPRE1_NODIV,
                .ppre2 = RCC_CFGR_PPRE2_NODIV,
        };
#endif
	const struct rcc_clock_scale myclock = {
                .pll_source = RCC_CFGR_PLLSRC_HSI_CLK,
                .pll_mul = RCC_CFGR_PLLMUL_MUL6,
                .pll_div = RCC_CFGR_PLLDIV_DIV3,
                .hpre = RCC_CFGR_HPRE_SYSCLK_NODIV,
                .ppre1 = RCC_CFGR_PPRE1_HCLK_NODIV,
                .ppre2 = RCC_CFGR_PPRE2_HCLK_NODIV,
                .voltage_scale = PWR_SCALE1,
                .flash_waitstates = 1,
                .ahb_frequency = 32e6,
                .apb1_frequency = 32e6,
                .apb2_frequency = 32e6,
        };

        rcc_clock_setup_pll(&myclock);
	hw_init();
	printf("starting freertos...\n");

	xTaskCreate(prvTaskGreenBlink1, "green.blink", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(prvTaskSpiSlave, "spi.slave", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

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

