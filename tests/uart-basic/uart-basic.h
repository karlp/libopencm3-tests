/*
 * Karl Palsson <karlp@tweak.net.au> Oct 2017
 * Considered to be available under your choice of:
 * BSD2 clause, Apache2, MIT, X11 or ISC licenses
 */

#pragma once

#include <stdint.h>
#include <libopencm3/stm32/usart.h>

struct ub_hw {
	/** usart itself, eg USART2 */
	uint32_t uart;
	/** RCC_xxx flag for this usart, eg RCC_USART2 */
	uint32_t uart_rcc;
	/** eg NVIC_USART2_IRQ */
	uint32_t uart_nvic;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise the uart itself.
 * gpios are required to have been already configured as needed
 * @param ub
 */
void ub_init(struct ub_hw *ub);

/**
 * Call this, it will "do stuff"
 */
void ub_task(void);

/**
 * Call this from your board irq handler, it will "do the right thing"
 */
void ub_irq_handler(void);

#ifdef __cplusplus
}
#endif
