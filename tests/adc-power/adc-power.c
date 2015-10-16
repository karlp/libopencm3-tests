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


void adc_power_init(void)
{
	/* Some basic ADC config, that we won't touch again */
	rcc_periph_clock_enable(RCC_ADC1);
	adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_28CYC);
#if 0
	// DANGER DANGER! doing this without DMA is dum.
	// but... we're busy polling, we should be right... right?
	// (dma across platforms is teh suck)
	adc_enable_scan_mode(ADC1);
	ADC_CR2 |= ADC_CR2_EOCS; // FIXME
#else
	adc_disable_scan_mode(ADC1);
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

void adc_power_task_up(void) {
	TIM_CNT(TIMER) = 0;
	adc_power_on(ADC1);
	unsigned int td = TIM_CNT(TIMER);
	
	/* just for kicks, let's time some sequences too....
	 * I mean, we're going to do some conversions right? */
	adc_set_single_conversion_mode(ADC1);
#if 0
	uint8_t channels[2] = { 0, 1 };
	adc_set_regular_sequence(ADC1, ARRAY_SIZE(channels), channels);
	TIM_CNT(TIMER) = 0;
	adc_start_conversion_regular(ADC1);
	while (!adc_eoc(ADC1));
	unsigned int v1 = adc_read_regular(ADC1);
	while (!adc_eoc(ADC1));
	unsigned int v2 = adc_read_regular(ADC1);
	unsigned int tconv = TIM_CNT(TIMER);
#else
	TIM_CNT(TIMER) = 0;
	unsigned int v1 = read_adc_naiive(0);
	unsigned int v2 = read_adc_naiive(1);
	unsigned int tconv = TIM_CNT(TIMER);
#endif
	
	printf("ton: %u, tconv: %u, v1: %u, v2: %u\n", td, tconv, v1, v2);
}

void adc_power_task_down()
{
	TIM_CNT(TIMER) = 0;
	adc_power_off(ADC1);
	unsigned int td = TIM_CNT(TIMER);
	printf("toff in: %u\n", td);
}