/*
 * Karl Palsson <karlp@tweak.au> Jan 2023
 */

#include <stdbool.h>
#include <stdint.h>

#pragma once

/* How is the spi port connected? */
struct hw_spi {
	uint32_t spi;
	uint32_t gpio_port;
	uint32_t rcc_spi;
	uint16_t pin_mosi;
	uint16_t pin_miso;
	uint16_t pin_sck;
	uint16_t pin_cs;
	uint8_t nvic_spi;
};

struct hw_detail {
        const struct rcc_clock_scale *clock_config;
        uint32_t led_port;
        uint16_t led_pin;
        const struct hw_spi *dut_spi;
};


extern struct hw_detail hw_details;
//extern const struct hw_uart hw_uart_mb_em3x;


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

#ifdef __cplusplus
}
#endif
