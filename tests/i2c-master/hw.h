/*
 * Feb 2017 Karl Palsson <karlp@tweak.net.au>
 */

#include <stdbool.h>
#include <stdint.h>

#pragma once

struct hw_detail
{
       uint32_t periph; /* eg: I2C1 */
       uint32_t periph_rcc; /* eg: RCC_I2C1 */
       uint32_t periph_rst; /* eg: RST_I2C1 */
       uint32_t pins; /* eg: GPIO8 | GPIO9 */ /* ASSUMES SAME PORT*/
       uint32_t port; /* eg GPIOB */
       uint32_t port_rcc; /* eg RCC_GPIOB */
};

extern struct hw_detail hw_details;

#ifdef __cplusplus
extern "C" {
#endif

/**
        * Expected to setup clocks, turn on all peripherals, and configure
        * any gpios necessary.
        * @param hw pointer to hw details necessary
        */
       void hw_setup(struct hw_detail* hw);

       /* let devices have a status led */
       void hw_set_led(bool val);

#ifdef __cplusplus
}
#endif
