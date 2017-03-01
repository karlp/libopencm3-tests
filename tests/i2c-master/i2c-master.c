/*
 * Feb 2017, Karl Palsson <karlp@tweak.net.au>
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include "trace.h"
#include "hw.h"
#include "i2c-master.h"

#define SENSOR_ADDRESS (0x40)

enum sht21_cmd_e {
	SHT21_CMD_TEMP_HOLD = 0xe3,
	SHT21_CMD_HUMIDITY_HOLD = 0xe5,
	SHT21_CMD_TEMP_NOHOLD = 0xf3,
	SHT21_CMD_HUMIDITY_NOHOLD = 0xf5,
	SHT21_CMD_WRITE_REG = 0xe6,
	SHT21_CMD_READ_REG = 0xe7,
	SHT21_CMD_RESET = 0xfe,
	/* 0xfa, 0x0f to read serial */
};

void i2cm_init(void)
{
	rcc_periph_clock_enable(hw_details.periph_rcc);
	rcc_periph_reset_pulse(hw_details.periph_rst);
	i2c_set_standard_mode(hw_details.periph);
	//	i2c_enable_ack(hw_details.periph); /* NO ACK FOR SHT21! */
	//i2c_set_dutycycle(hw_details.periph, I2C_CCR_DUTY_DIV2); /* default, no need to do this really */

	i2c_set_clock_frequency(hw_details.periph, hw_details.i2c_clock_megahz);
	/* x Mhz / (100kHz * 2) */
	i2c_set_ccr(hw_details.periph, hw_details.i2c_clock_megahz * 5);
	/* Sm mode, (100kHz) freqMhz + 1 */
	i2c_set_trise(hw_details.periph, hw_details.i2c_clock_megahz + 1);

	i2c_peripheral_enable(hw_details.periph);
}

static void sht21_send_data(uint32_t i2c, size_t n, uint8_t *data)
{
	while ((I2C_SR2(i2c) & I2C_SR2_BUSY)) {
	}

	i2c_send_start(i2c);

	/* Wait for master mode selected */
	while (!((I2C_SR1(i2c) & I2C_SR1_SB)
		& (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

	i2c_send_7bit_address(i2c, SENSOR_ADDRESS, I2C_WRITE);

	/* Waiting for address is transferred. */
	while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));

	/* Cleaning ADDR condition sequence. */
	uint32_t reg32 = I2C_SR2(i2c);
	(void) reg32; /* unused */

	size_t i;
	for (i = 0; i < n; i++) {
		i2c_send_data(i2c, data[i]);
		while (!(I2C_SR1(i2c) & (I2C_SR1_BTF)));
	}
}

static void sht21_send_cmd(uint32_t i2c, uint8_t cmd)
{
	while ((I2C_SR2(i2c) & I2C_SR2_BUSY)) {
	}

	i2c_send_start(i2c);

	/* Wait for master mode selected */
	while (!((I2C_SR1(i2c) & I2C_SR1_SB)
		& (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

	i2c_send_7bit_address(i2c, SENSOR_ADDRESS, I2C_WRITE);

	/* Waiting for address is transferred. */
	while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));

	/* Cleaning ADDR condition sequence. */
	uint32_t reg32 = I2C_SR2(i2c);
	(void) reg32; /* unused */

	i2c_send_data(i2c, cmd);
	while (!(I2C_SR1(i2c) & (I2C_SR1_BTF)));
}

static void sht21_readn(uint32_t i2c, int n, uint8_t *res)
{
	//assert(n > 0);
	i2c_send_start(i2c);

	i2c_enable_ack(i2c);

	/* Wait for master mode selected */
	while (!((I2C_SR1(i2c) & I2C_SR1_SB)
		& (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

	i2c_send_7bit_address(i2c, SENSOR_ADDRESS, I2C_READ);

	/* Waiting for address is transferred. */
	while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));
	/* Cleaning ADDR condition sequence. */
	uint32_t reg32 = I2C_SR2(i2c);
	(void) reg32; /* unused */

	int i = 0;
	for (i = 0; i < n; ++i) {
		if (i == n - 1) {
			i2c_disable_ack(i2c);
		}
		while (!(I2C_SR1(i2c) & I2C_SR1_RxNE));
		res[i] = i2c_get_data(i2c);
	}
	i2c_send_stop(i2c);

	return;
}

static float sht21_convert_temp(uint16_t raw)
{
	//assert((raw & 0x2) == 0x2);
	raw &= ~0x3; /* Clear lower status bits */
	float tf = -46.85 + 175.72 * ((float) raw / 65536.0);
	return tf;
}

static float sht21_convert_humi(uint16_t raw)
{
	//assert((raw & 0x2) == 0);
	raw &= ~0x3; /* Clear lower status bits */
	float tf = -6 + 125 * ((float) raw / 65536.0);
	return tf;
}

static float sht21_read_temp_hold(uint32_t i2c)
{
	//        gpio_set(LED_DISCO_BLUE_PORT, LED_DISCO_BLUE_PIN);
	sht21_send_cmd(i2c, SHT21_CMD_TEMP_HOLD);

	uint8_t data[3];
	sht21_readn(i2c, 2, data);
	uint8_t crc = data[2];
	uint16_t temp = data[0] << 8 | data[1];
	printf("CRC=%#x, data0=%#x, data1=%#x\n", crc, data[0], data[1]);
	//        gpio_clear(LED_DISCO_BLUE_PORT, LED_DISCO_BLUE_PIN);
	return sht21_convert_temp(temp);
}

static float sht21_read_humi_hold(uint32_t i2c)
{
	//        gpio_set(LED_DISCO_BLUE_PORT, LED_DISCO_BLUE_PIN);
	sht21_send_cmd(i2c, SHT21_CMD_HUMIDITY_HOLD);

	uint8_t data[3];
	sht21_readn(i2c, 2, data);
	uint8_t crc = data[2];
	uint16_t left = data[0] << 8 | data[1];
	printf("CRC=%#x, data0=%#x, data1=%#x\n", crc, data[0], data[1]);

	//        gpio_clear(LED_DISCO_BLUE_PORT, LED_DISCO_BLUE_PIN);
	return sht21_convert_humi(left);
}

static void sht21_readid(void)
{
	sht21_send_cmd(I2C1, SHT21_CMD_READ_REG);
	uint8_t raw = 0;
	sht21_readn(I2C1, 1, &raw);
	printf("raw user reg = %#x\n", raw);
	int resolution = ((raw & 0x80) >> 6) | (raw & 1);
	printf("temp resolution is in %d bits\n", 14 - resolution);
	printf("battery status: %s\n", (raw & (1 << 6) ? "failing" : "good"));
	printf("On chip heater: %s\n", (raw & 0x2) ? "on" : "off");

	uint8_t req1[] = {0xfa, 0x0f};
	uint8_t res[8];
	sht21_send_data(I2C1, 2, req1);
	sht21_readn(I2C1, sizeof(res), res);
	uint8_t req2[] = {0xfc, 0xc9};
	uint8_t res2[8];
	sht21_send_data(I2C1, 2, req2);
	sht21_readn(I2C1, sizeof(res), res2);
	printf("Serial = %02x%02x %02x%02x %02x%02x %02x%02x\n",
		res2[3], res2[4], res[0], res[2], res[4], res[6], res2[0], res2[1]);
}

void i2cm_task(void)
{
	gpio_set(hw_details.trigger_port, hw_details.trigger_pin);
	sht21_readid();
	float temp = sht21_read_temp_hold(I2C1);
	float humi = sht21_read_humi_hold(I2C1);
	gpio_clear(hw_details.trigger_port, hw_details.trigger_pin);
	printf("Temp: %f C, RH: %f\n", temp, humi);

}