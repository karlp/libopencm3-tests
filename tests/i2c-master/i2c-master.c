/*
 * Feb 2017, Karl Palsson <karlp@tweak.net.au>
 */

#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/rcc.h>
#include "hw.h"
#include "i2c-master.h"


void i2cm_init(void) {
	rcc_periph_clock_enable(hw_details.periph_rcc);
	rcc_periph_reset_pulse(hw_details.periph_rst);
	i2c_set_standard_mode(hw_details.periph);
	i2c_enable_ack(hw_details.periph);
	//i2c_set_dutycycle(hw_details.periph, I2C_CCR_DUTY_DIV2); /* default, no need to do this really */
	i2c_set_clock_frequency(hw_details.periph, I2C_CR2_FREQ_42MHZ);
	/* 42MHz / (100kHz * 2) */
	i2c_set_ccr(hw_details.periph, 210);
	/* standard mode, freqMhz+1*/
	i2c_set_trise(hw_details.periph, 43);
	i2c_peripheral_enable(hw_details.periph);
}