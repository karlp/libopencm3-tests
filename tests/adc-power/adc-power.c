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
#if defined(STM32F0)
#define SEPARATE_ADC_SAMPLING 0
#define SAMPLE_TIME_BASIC ADC_SMPR_SMP_239DOT5 // 4usec or more for tempsensor
#define HAS_CALIBRATION 1
#elif defined(STM32F1)
#define SEPARATE_ADC_SAMPLING 0
#define SAMPLE_TIME_BASIC ADC_SMPR_SMP_28DOT5CYC // 17usecs or more. >~15cycles at 9Mhz
#define SEPARATE_VREF 0
#define HAS_CALIBRATION 1
#define HAS_ROM_CALIBRATION 0
#elif defined(STM32F3)
#define SAMPLE_TIME_BASIC ADC_SMPR_SMP_181DOT5CYC
#define SAMPLE_TIME_TEMP ADC_SMPR_SMP_601DOT5CYC // 2.2usecs or more
#define SAMPLE_TIME_VREF SAMPLE_TIME_TEMP
#define HAS_CALIBRATION 1
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
#elif defined(STM32L4)
#define SAMPLE_TIME_BASIC ADC_SMPR_SMP_247DOT5CYC
#define SAMPLE_TIME_TEMP ADC_SMPR_SMP_247DOT5CYC
#define SAMPLE_TIME_VREF SAMPLE_TIME_TEMP
#define HAS_CALIBRATION 1
#else
#error "no sample time for your target yet?!"
#endif

#ifndef SEPARATE_VREF
#define SEPARATE_VREF 1
#endif
#ifndef SEPARATE_ADC_SAMPLING
#define SEPARATE_ADC_SAMPLING 1
#endif
#ifndef HAS_CALIBRATION
#define HAS_CALIBRATION 0
#endif
#ifndef HAS_ROM_CALIBRATION
#define HAS_ROM_CALIBRATION 1
#endif


void adc_power_init(void)
{
	/* Some basic ADC config, that we won't touch again */
#if defined(STM32F3)
	/* silly f303 */
	rcc_periph_clock_enable(RCC_ADC12);
	rcc_adc_prescale(RCC_CFGR2_ADCxPRES_PLL_CLK_DIV_1, RCC_CFGR2_ADCxPRES_PLL_CLK_DIV_1);
	adc_enable_regulator(ADC1);
#elif defined (STM32L4)
	/* same same but different */
	rcc_periph_clock_enable(RCC_ADC1);
	ADC_CR(ADC1) &= ~ADC_CR_DEEPPWD;
	RCC_CCIPR |= 3 << 28; // system clock as adc clock.  (with CKMODE == 0)
	adc_enable_regulator(ADC1);
	
	ADC_CR(ADC1) &= ~ADC_CR_ADCALDIF;
	ADC_CR(ADC1) |= ADC_CR_ADCAL;
	while (ADC_CR(ADC1) & ADC_CR_ADCAL);
	

#else
	rcc_periph_clock_enable(RCC_ADC1);
#if 0
	// DANGER DANGER! doing this without DMA is dum.
	// but... we're busy polling, we should be right... right?
	// (dma across platforms is teh suck)
	adc_enable_scan_mode(ADC1);
	ADC_CR2 |= ADC_CR2_EOCS; // FIXME
#else
	// FIXME - f0!  adc_disable_scan_mode(ADC1);
#endif
	
#endif
#if (SEPARATE_ADC_SAMPLING == 1)
	adc_set_sample_time_on_all_channels(ADC1, SAMPLE_TIME_BASIC);
	adc_set_sample_time(ADC1, ADC_CHANNEL_TEMP, SAMPLE_TIME_TEMP);
	adc_set_sample_time(ADC1, ADC_CHANNEL_TEMP, SAMPLE_TIME_VREF);
#else
	adc_set_sample_time_on_all_channels(ADC1, SAMPLE_TIME_BASIC);
#endif
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
	// FIXME - use a trigger, see f1 notes!
#if defined (STM32F1)
	adc_start_conversion_direct(ADC1);
#else
	adc_start_conversion_regular(ADC1);
#endif
	while (!adc_eoc(ADC1));
	return adc_read_regular(ADC1);
}

static float adc_calc_tempf(unsigned int ts_v, unsigned int vref) {
#if (HAS_ROM_CALIBRATION == 1)
	float adjusted_vtemp = ts_v * ST_VREFINT_CAL * 1.0f / vref * 1.0f;
	float slope = (110-30) * 1.0f / (ST_TSENSE_CAL2_110C - ST_TSENSE_CAL1_30C) * 1.0f;
	return slope * (adjusted_vtemp - ST_TSENSE_CAL1_30C) + 30;
#else
	(void)vref;
	return ts_v * 1.0;
#endif
}

static int adc_calc_tempi(unsigned int ts, unsigned int vref) {
#if (HAS_ROM_CALIBRATION == 1)
	int adjusted_vtemp = ts * ST_VREFINT_CAL / vref;
	int slope = (110-30) / (ST_TSENSE_CAL2_110C - ST_TSENSE_CAL1_30C);
	return slope * (adjusted_vtemp - ST_TSENSE_CAL1_30C) + 30;
#else
	(void)vref;
	return ts * 1.00;
#endif
}

void adc_power_task_up(void) {
	TIM_CNT(TIMER) = 0;
	// Welcome to f1 world.
#if defined (STM32F1)
	adc_power_on(ADC1);
	for (int i = 0; i < 0x80000; i++) { /* Wait a bit. */
		__asm__("NOP");
	}
#endif
#if (HAS_CALIBRATION == 1)
	adc_calibrate(ADC1);
#endif
	adc_power_on(ADC1);
	unsigned int td = TIM_CNT(TIMER);
	
	/* just for kicks, let's time some sequences too....
	 * I mean, we're going to do some conversions right? */
	adc_set_single_conversion_mode(ADC1);
	TIM_CNT(TIMER) = 0;
	unsigned int v1 = read_adc_naiive(1); // 8 for l4!
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
	/* double power off should be safe */
	adc_power_off(ADC1);
	unsigned int td = TIM_CNT(TIMER);
	printf("toff in: %u\n", td);
}
