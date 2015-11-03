/**
 * Testing ADC power up and power down, for timing and actual on/off
 * Uses TIM6, because DWT_CYCCNT not available on cm0(+) :(
 * (And with no DWT, no ITM timestamping either, get a real mcu!)
 */

#include <stdio.h>
#include <stdint.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include "adc-power.h"

/* Everyone has tim6 right? */
#define TIMER TIM6
#define TIMER_RCC RCC_TIM6

// TODO - stick this in libopencm3?
#define	ARRAY_SIZE(a)	(sizeof((a)) / sizeof((a)[0]))

// Still have some bad shit to deal with...
#if defined(STM32F3)
#define SAMPLE_TIME_BASIC ADC_SMPR1_SMP_181DOT5CYC
#define SAMPLE_TIME_TEMP ADC_SMPR1_SMP_601DOT5CYC // 2.2usecs or more
#define SAMPLE_TIME_VREF SAMPLE_TIME_TEMP
#elif defined(STM32F4)
#define SAMPLE_TIME_BASIC ADC_SMPR_SMP_28CYC
#define SAMPLE_TIME_TEMP ADC_SMPR_SMP_144CYC // 10 usecs or more, in theory needs 840cycles!
#define SAMPLE_TIME_VREF SAMPLE_TIME_TEMP
#define ADC_CHANNEL_TEMP ADC_CHANNEL_TEMP_F40
#define SEPARATE_VREF 0
#elif defined(STM32L1)
#define SAMPLE_TIME_BASIC ADC_SMPR_SMP_24CYC
#define SAMPLE_TIME_TEMP ADC_SMPR_SMP_192CYC
#define SAMPLE_TIME_VREF SAMPLE_TIME_TEMP
#define SEPARATE_VREF 0
#else
#error "no sample time for your target yet?!"
#endif

#ifndef SEPARATE_VREF
#define SEPARATE_VREF 1
#endif


void adc_power_init(void)
{
	/* Some basic ADC config, that we won't touch again */
#if defined(STM32F3)
	/* silly f303 */
	rcc_periph_clock_enable(RCC_ADC12);
	rcc_adc_prescale(RCC_CFGR2_ADCxPRES_PLL_CLK_DIV_1, RCC_CFGR2_ADCxPRES_PLL_CLK_DIV_1);
	adc_enable_regulator(ADC1);
#else
	rcc_periph_clock_enable(RCC_ADC1);
#if 0
	// DANGER DANGER! doing this without DMA is dum.
	// but... we're busy polling, we should be right... right?
	// (dma across platforms is teh suck)
	adc_enable_scan_mode(ADC1);
	ADC_CR2 |= ADC_CR2_EOCS; // FIXME
#else
	adc_disable_scan_mode(ADC1);
#endif
	
#endif
	adc_set_sample_time_on_all_channels(ADC1, SAMPLE_TIME_BASIC);
	adc_set_sample_time(ADC1, ADC_CHANNEL_TEMP, SAMPLE_TIME_TEMP);
	adc_set_sample_time(ADC1, ADC_CHANNEL_TEMP, SAMPLE_TIME_VREF);
	adc_enable_temperature_sensor();
#if (SEPARATE_VREF == 1)
	adc_enable_vrefint();
#endif

	/*
	 * We're going to setup a timer to run at top speed, so... "fast" 
	 * but we don't actually care about the rate itself.  We just 
	 * want to collect how many ticks it takes to enable and disable
	 * the adc.
	 */
	rcc_periph_clock_enable(TIMER_RCC);
	timer_reset(TIMER);
	timer_set_prescaler(TIMER, 0);
	timer_enable_counter(TIMER);
}

static uint16_t read_adc_naiive(uint8_t channel)
{
	uint8_t channel_array[16];
	channel_array[0] = channel;
	adc_set_regular_sequence(ADC1, 1, channel_array);
	adc_start_conversion_regular(ADC1);
	while (!adc_eoc(ADC1));
	return adc_read_regular(ADC1);
}

static float adc_calc_tempf(unsigned int ts_v, unsigned int vref) {
	float adjusted_vtemp = ts_v * ST_VREFINT_CAL * 1.0 / vref * 1.0;
	float slope = (110-30) * 1.0 / (ST_TSENSE_CAL2_110C - ST_TSENSE_CAL1_30C) * 1.0;
	return slope * (adjusted_vtemp - ST_TSENSE_CAL1_30C) + 30;
}

static float adc_calc_tempi(uint16_t ts, uint16_t vref) {
	int adjusted_vtemp = ts * ST_VREFINT_CAL / vref;
	int slope = (110-30) / (ST_TSENSE_CAL2_110C - ST_TSENSE_CAL1_30C);
	return slope * (adjusted_vtemp - ST_TSENSE_CAL1_30C) + 30;
}

void adc_power_task_up(void) {
	TIM_CNT(TIMER) = 0;
	adc_power_on(ADC1);
	unsigned int td = TIM_CNT(TIMER);
	
	/* just for kicks, let's time some sequences too....
	 * I mean, we're going to do some conversions right? */
	adc_set_single_conversion_mode(ADC1);
	TIM_CNT(TIMER) = 0;
	unsigned int v1 = read_adc_naiive(1);
	unsigned int v5 = read_adc_naiive(5);
	unsigned int temp_adc = read_adc_naiive(ADC_CHANNEL_TEMP);
	unsigned int vref_adc = read_adc_naiive(ADC_CHANNEL_VREF);
	unsigned int tconv = TIM_CNT(TIMER);
	TIM_CNT(TIMER) = 0;
	float tempf = adc_calc_tempf(temp_adc, vref_adc);
	int tconvf = TIM_CNT(TIMER);
	TIM_CNT(TIMER) = 0;
	int tempi = adc_calc_tempi(temp_adc, vref_adc);
	int tconvi = TIM_CNT(TIMER);
	
	printf("ton: %u, tconv: %u, ch1: %u, ch5: %u\n", td, tconv, v1, v5);
	printf("\tTemperature: %f (tc %d) or %d (tc %d) (raw ts: %d, raw vref: %d)\n",
		tempf, tconvf, tempi, tconvi, temp_adc, vref_adc);
	//printf("\tT1-30 %d, t2-110 %d, vrefcal: %d\n", ST_TSENSE_CAL1_30C, ST_TSENSE_CAL2_110C, ST_VREFINT_CAL);
}

void adc_power_task_down()
{
	TIM_CNT(TIMER) = 0;
	adc_power_off(ADC1);
	unsigned int td = TIM_CNT(TIMER);
	printf("toff in: %u\n", td);
}