/*
 * March 2017 Karl Palsson <karlp@tweak.net.au>
 */

#include <stdbool.h>
#include <stdint.h>

#pragma once

/* How is the modbus port connected? */
struct hw_uart {
	uint32_t usart;
	uint32_t gpio_port;
	uint32_t rcc_usart;
	uint16_t pin_rx;
	uint16_t pin_tx;
	uint8_t nvic_usart;
};

struct hw_detail {
        const struct rcc_clock_scale *clock_config;
        uint16_t product_code;
        uint32_t led_port;
        uint16_t led_pin;
        uint32_t rs485de_port;
        uint16_t rs485de_pin;
        const struct hw_uart *mb_port;
};


extern struct hw_detail hw_details;
extern const struct hw_uart hw_uart_mb_em3x;


#ifdef __cplusplus
extern "C" {
#endif
	/**
	 * Expected to setup clocks, turn on all peripherals, and configure
	 * any gpios necessary.
	 * @param hw pointer to hw details necessary
	 */
	void hw_setup(void);

	/* let devices have a status led */
	void hw_set_status_led(bool val);
	void hw_set_rs485de(bool val);

	/* Provided by the modbus port layer    */
	void hw_mb_port_irq(void);

#ifdef __cplusplus
}
#endif
