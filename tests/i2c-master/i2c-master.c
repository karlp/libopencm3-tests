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

	i2c_set_speed(hw_details.periph, i2c_speed_sm_100k, hw_details.i2c_clock_megahz);
	//i2c_set_speed(hw_details.periph, i2c_speed_fm_400k, hw_details.i2c_clock_megahz);

	i2c_peripheral_enable(hw_details.periph);
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
	uint8_t data[3];
	uint8_t cmd = SHT21_CMD_TEMP_HOLD;
	i2c_transfer7(i2c, SENSOR_ADDRESS, &cmd, 1, data, sizeof(data));
	uint8_t crc = data[2];
	uint16_t temp = data[0] << 8 | data[1];
	// TODO - calcualte CRC and check!
	printf("CRC=%#x, data0=%#x, data1=%#x\n", crc, data[0], data[1]);
	return sht21_convert_temp(temp);
}

static float sht21_read_humi_hold(uint32_t i2c)
{
	uint8_t data[3];
	uint8_t cmd = SHT21_CMD_HUMIDITY_HOLD;
	i2c_transfer7(i2c, SENSOR_ADDRESS, &cmd, 1, data, sizeof(data));

	uint8_t crc = data[2];
	uint16_t left = data[0] << 8 | data[1];
	// TODO - calcualte CRC and check!
	printf("CRC=%#x, data0=%#x, data1=%#x\n", crc, data[0], data[1]);
	return sht21_convert_humi(left);
}

static void sht21_readid(void)
{
	uint8_t raw = 0;
	uint8_t cmd = SHT21_CMD_READ_REG;
	printf("RP...");
	i2c_transfer7(hw_details.periph, SENSOR_ADDRESS, &cmd, 1, &raw, 1);
	printf("..S/S\n");
	// or, with stop/start
	i2c_transfer7(hw_details.periph, SENSOR_ADDRESS, &cmd, 1, 0, 0);
	i2c_transfer7(hw_details.periph, SENSOR_ADDRESS, 0, 0, &raw, 1);

	printf("raw user reg = %#x\n", raw);
	int resolution = ((raw & 0x80) >> 6) | (raw & 1);
	printf("temp resolution is in %d bits\n", 14 - resolution);
	printf("battery status: %s\n", (raw & (1 << 6) ? "failing" : "good"));
	printf("On chip heater: %s\n", (raw & 0x2) ? "on" : "off");

	uint8_t req1[] = {0xfa, 0x0f};
	uint8_t res[8];
	i2c_transfer7(hw_details.periph, SENSOR_ADDRESS, req1, sizeof(req1), res, 8);
	uint8_t req2[] = {0xfc, 0xc9};
	uint8_t res2[8];
	i2c_transfer7(hw_details.periph, SENSOR_ADDRESS, req1, sizeof(req1), res2, 8);

	printf("Serial = %02x%02x %02x%02x %02x%02x %02x%02x\n",
		res2[3], res2[4], res[0], res[2], res[4], res[6], res2[0], res2[1]);
}

void i2cm_task(void)
{
	static int i = 1;
	printf(">>>>Starting iteration %d\n", i++);
	gpio_set(hw_details.trigger_port, hw_details.trigger_pin);
	sht21_readid();
	float temp = sht21_read_temp_hold(hw_details.periph);
	float humi = sht21_read_humi_hold(hw_details.periph);
	gpio_clear(hw_details.trigger_port, hw_details.trigger_pin);
	printf("Temp: %f C, RH: %f\n", temp, humi);

}