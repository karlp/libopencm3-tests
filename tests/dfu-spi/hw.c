/*
 * Karl Palsson <karlp@etactica.com> October 2020
 * Common file handling hw layer changes.
 */

#include "hw.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

//#include <remake/product_codes.h>

/* !!! a global struct hw_detail hw_details is expected to exist! */

const struct hw_uart hw_uart_mb_em3x = {
	.gpio_port = GPIOA,
	.pin_rx = GPIO3,
	.pin_tx = GPIO2,
	.rcc_usart = RCC_USART2,
	.usart = USART2,
	.nvic_usart = NVIC_USART2_IRQ,
};


void hw_setup(void)
{

	rcc_clock_setup_pll(hw_details.clock_config);

	/* Turn on all peripherals needed by all variants */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	/* syscfg for exti */
	rcc_periph_clock_enable(RCC_SYSCFG);
	/* Backup registers and things for bootloader comms */
	rcc_periph_clock_enable(RCC_PWR);
	/* CRC engine is used for the serial number */
	rcc_periph_clock_enable(RCC_CRC);

	/* Configure GPIO for the status LED */
	gpio_mode_setup(hw_details.led_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, hw_details.led_pin);
}

void hw_set_status_led(bool val)
{
	if (val) {
		gpio_set(hw_details.led_port, hw_details.led_pin);
	} else {
		gpio_clear(hw_details.led_port, hw_details.led_pin);
	}
}

void hw_set_rs485de(bool val)
{
	if (!hw_details.rs485de_pin) {
		return;
	}
	if (val) {
		gpio_set(hw_details.rs485de_port, hw_details.rs485de_pin);
	} else {
		gpio_clear(hw_details.rs485de_port, hw_details.rs485de_pin);
	}
}
