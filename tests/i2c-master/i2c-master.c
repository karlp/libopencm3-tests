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

// ------------------ section proposed to go up to libopencm3

enum i2c_speeds {
	i2c_speed_sm_100k,
	i2c_speed_fm_400k,
	i2c_speed_fmp_1m,
	i2c_speed_unknown
};

/* to go to i2c-v1 impl file, with common name.... */
static void i2c_set_speed_v1(uint32_t p, enum i2c_speeds speed, uint32_t clock_megahz)
{
#if defined(I2C_SR2)
	switch(speed) {
	case i2c_speed_fm_400k:
		// FIXME
		printf("oops, haven't gotten 400k yet!, grab chucks code from pr470!");
		break;
	default:
		/* fall back to standard mode */
	case i2c_speed_sm_100k:
		i2c_set_clock_frequency(p, clock_megahz);
		i2c_set_standard_mode(p);
		/* x Mhz / (100kHz * 2) */
		i2c_set_ccr(p, clock_megahz * 5);
		/* Sm mode, (100kHz) freqMhz + 1 */
		i2c_set_trise(p, clock_megahz + 1);
		break;
	}
#else
	(void)p;
	(void)speed;
	(void)clock_megahz;
#endif
}

/* to go to i2c-v2 impl file, with common name.... */
static void i2c_set_speed_v2(uint32_t p, enum i2c_speeds speed, uint32_t clock_megahz)
{
#if !defined(I2C_SR2)
	int prescaler;
	switch(speed) {
	case i2c_speed_fmp_1m:
	case i2c_speed_fm_400k:
		// FIXME
		printf("oops, haven't gotten to those speeds yet!");
		break;
	default:
		/* fall back to standard mode */
	case i2c_speed_sm_100k:
		/* target 2Mhz input, so tpresc = 500ns */
		prescaler = clock_megahz / 2 - 1;
		i2c_set_prescaler(p, prescaler);
		i2c_set_scl_low_period(p, 9); // 5usecs
		i2c_set_scl_high_period(p, 7); // 4usecs
		i2c_set_data_hold_time(p, 1); // 0.5usecs
		i2c_set_data_setup_time(p, 2); // 1.25usecs
		break;
	}
#endif
}


/* requires clock to be a multiple of 2 ! */
/**
 * Set the i2c line speed as optimally as possible
 * @param p
 * @param speed
 * @param clock_megahz  _normally_ provide: rcc_apb1_frequency / 1000000 here
 * TODO: clock must be a multiple of 2meg for -v2 at least, improve docs here!
 */
static void i2c_set_speed(uint32_t p, enum i2c_speeds speed, uint32_t clock_megahz)
{
#if defined I2C_SR2
	i2c_set_speed_v1(p, speed, clock_megahz);
#else
	i2c_set_speed_v2(p, speed, clock_megahz);
#endif
}

// read/write are almost identical!
//static void i2c__transfer_helper(uint32_p, uint8_t addr, uint8_t *buf)
static
void i2c_transfer7(uint32_t i2c, uint8_t addr, uint8_t *w, size_t wn, uint8_t *r, size_t rn)
{
	int wait;
	size_t i;
	/*  waiting for busy is unnecessary. read the RM */
	if (wn) {
		i2c_set_7bit_address(i2c, addr);
		i2c_set_write_transfer_dir(i2c);
		i2c_set_bytes_to_transfer(i2c, wn);
		if (rn) {
			i2c_disable_autoend(i2c);
		} else {
			i2c_enable_autoend(i2c);
		}
		i2c_send_start(i2c);

		while (wn--) {
			wait = true;
			while (wait) {
				if (i2c_transmit_int_status(i2c)) {
					wait = false;
				}
				while (i2c_nack(i2c)); /* FIXME Some error */
			}
			i2c_send_data(i2c, *w++);
		}
		/* not entirely sure this is really necessary.
		 * RM implies it will stall until it can write out the later bits
		 */
		if (rn) {
			while (!i2c_transfer_complete(i2c));
		}
	}

	if (rn) {
		/*Setting transfer properties*/
		i2c_set_7bit_address(i2c, addr);
		i2c_set_read_transfer_dir(i2c);
		i2c_set_bytes_to_transfer(i2c, rn);
		/*start transfer*/
		i2c_send_start(i2c);
		/* important to do it afterwards to do a proper repeated start! */
		i2c_enable_autoend(i2c);

		for (i = 0; i < rn; i++) {
			while (i2c_received_data(i2c) == 0);
			r[i] = i2c_get_data(i2c);
		}
	}

}

/**
 * Write bytes to a 7bit address
 * @param p i2c peripheral of interest
 * @param addr 7bit i2c slave address (unshifted)
 * @param buf data to send
 * @param count how many bytes to send
 * @param end true if you wish to send a stop bit. false if you intend to
 * continue sending data with a repeated start
 */
static
void i2c_write_bytes7(uint32_t p, uint8_t addr, uint8_t *buf, size_t count, bool end)
{
	bool wait;
	size_t i;
	/* start transfer */
	i2c_send_start(p);
	while (i2c_busy(p) == 1);
//	while (i2c_is_start(p) == 1);
	/* Setting transfer properties */
	i2c_set_bytes_to_transfer(p, count);
	i2c_set_7bit_address(p, addr);
	i2c_set_write_transfer_dir(p);
	if (end) {
		i2c_enable_autoend(p);
	} else {
		i2c_disable_autoend(p);
	}

	for (i = 0; i < count; i++) {
		wait = true;
		while (wait) {
			if (i2c_transmit_int_status(p)) {
				wait = false;
			}
			while (i2c_nack(p));
		}
		i2c_send_data(p, buf[i]);
	}
}

static
void i2c_read_bytes7(uint32_t p, uint8_t addr, uint8_t *buf, size_t count, bool end)
{
	bool wait;
	size_t i;
	//while (i2c_busy(p) == 1);  // FIXME - repeated start vs not
	//while (i2c_is_start(p) == 1);
	/* Setting transfer properties */
	i2c_set_bytes_to_transfer(p, 1);
	i2c_set_7bit_address(p, addr);
	i2c_set_read_transfer_dir(p);
	if (end) {
		i2c_enable_autoend(p);
	} else {
		i2c_disable_autoend(p);
	}
	/* start transfer */
	i2c_send_start(p);

	for (i = 0; i < count; i++) {
		while (i2c_received_data(p) == 0);
		buf[i] = i2c_get_data(p);
	}
}


// --------------- end of upstream planned section

void i2cm_init(void)
{
	rcc_periph_clock_enable(hw_details.periph_rcc);
	rcc_periph_reset_pulse(hw_details.periph_rst);
	//	i2c_enable_ack(hw_details.periph); /* NO ACK FOR SHT21! */

	i2c_set_speed(hw_details.periph, i2c_speed_sm_100k, hw_details.i2c_clock_megahz);

	i2c_peripheral_enable(hw_details.periph);
}

#if defined I2C_SR2
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

// FIXME - this is dumb, just need send_data with len==1!
static void sht21_send_cmd(uint32_t i2c, uint8_t cmd)
{
	while ((I2C_SR2(i2c) & I2C_SR2_BUSY)) {
	}   // wait for not busy!

	i2c_send_start(i2c);

	/* Wait for master mode selected */
	while (!((I2C_SR1(i2c) & I2C_SR1_SB)  // waiting for start bit to hav ebeen sent
		& (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));  // and waiting for no longer busy again

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
#endif

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
#if 0
	sht21_send_cmd(i2c, SHT21_CMD_TEMP_HOLD);
	sht21_readn(i2c, 2, data);
#endif
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
#if 0
	sht21_send_cmd(i2c, SHT21_CMD_HUMIDITY_HOLD);
	sht21_readn(i2c, 2, data);
#endif
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
#if defined I2C_SR2
	sht21_send_cmd(I2C1, SHT21_CMD_READ_REG);
	sht21_readn(I2C1, 1, &raw);
#else
	uint8_t cmd = SHT21_CMD_READ_REG;
	printf("RP...");
	i2c_transfer7(hw_details.periph, SENSOR_ADDRESS, &cmd, 1, &raw, 1);
	printf("..S/S\n");
	// or, with stop/start
	i2c_transfer7(hw_details.periph, SENSOR_ADDRESS, &cmd, 1, 0, 0);
	i2c_transfer7(hw_details.periph, SENSOR_ADDRESS, 0, 0, &raw, 1);

#endif
	printf("raw user reg = %#x\n", raw);
	int resolution = ((raw & 0x80) >> 6) | (raw & 1);
	printf("temp resolution is in %d bits\n", 14 - resolution);
	printf("battery status: %s\n", (raw & (1 << 6) ? "failing" : "good"));
	printf("On chip heater: %s\n", (raw & 0x2) ? "on" : "off");

	uint8_t req1[] = {0xfa, 0x0f};
	uint8_t res[8];
//	sht21_send_data(I2C1, 2, req1);
//	sht21_readn(I2C1, sizeof(res), res);
	i2c_transfer7(hw_details.periph, SENSOR_ADDRESS, req1, sizeof(req1), res, 8);
	uint8_t req2[] = {0xfc, 0xc9};
	uint8_t res2[8];
//	sht21_send_data(I2C1, 2, req2);
//	sht21_readn(I2C1, sizeof(res), res2);
	i2c_transfer7(hw_details.periph, SENSOR_ADDRESS, req1, sizeof(req1), res2, 8);

	printf("Serial = %02x%02x %02x%02x %02x%02x %02x%02x\n",
		res2[3], res2[4], res[0], res[2], res[4], res[6], res2[0], res2[1]);
}

void i2cm_task(void)
{
	static int i = 1;
	printf("Starting iteration %d\n", i++);
	gpio_set(hw_details.trigger_port, hw_details.trigger_pin);
	sht21_readid();
	float temp = sht21_read_temp_hold(hw_details.periph);
	float humi = sht21_read_humi_hold(hw_details.periph);
	gpio_clear(hw_details.trigger_port, hw_details.trigger_pin);
	printf("Temp: %f C, RH: %f\n", temp, humi);

}