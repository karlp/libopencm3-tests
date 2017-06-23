/**
 * Testing AWD interrupt, with timer triggered dma
 */

#include <stdio.h>
#include <stdint.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include "awd-timer-dma.h"

/* Everyone has tim6 right? */
#define TIMER TIM6
#define TIMER_RCC RCC_TIM6

/* what external channel should be included? (Connect a pot here) */
#define EXT_CHAN_RCC  RCC_GPIOA
#define EXT_CHAN_PORT  GPIOA
#define EXT_CHAN_PIN  GPIO5
#define EXT_CHAN_CHAN  5

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
#define SAMPLE_TIME_BASIC ADC_SMPR_SMP_48CYC
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

/* dma destination */
static uint16_t adc_buffer[3];
static int awd_hits;

/**
 * set up a timer from a 100khz timer base.
 * @param sample_rate minimum is ~2hz, max is just under 100khz
 */
static void timer_trigger_setup(int sample_rate)
{
        uint32_t timer = TIMER;

        timer_disable_counter(timer);
        TIM_CNT(timer) = 0;
	/* 100kHz base */
        timer_set_prescaler(timer, (rcc_apb1_frequency / (100*1000)) -1);
        timer_set_period(timer, ((100*1000)/sample_rate) - 1);
        timer_set_master_mode(timer, TIM_CR2_MMS_UPDATE);
}

void adc1_isr(void) {
	if (ADC_SR(ADC1) & ADC_SR_AWD) {
		ADC_SR(ADC1) &= ~ADC_SR_AWD;
		awd_hits++;
	}
}


void task_init(void)
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
	rcc_periph_clock_enable(RCC_DMA1);
	rcc_periph_clock_enable(RCC_TIM6);
	rcc_periph_clock_enable(EXT_CHAN_RCC);
	gpio_mode_setup(EXT_CHAN_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, EXT_CHAN_PIN);
	adc_power_off(ADC1);

	
#endif
#if (SEPARATE_ADC_SAMPLING == 1)
	adc_set_sample_time_on_all_channels(ADC1, SAMPLE_TIME_BASIC);
	adc_set_sample_time(ADC1, ADC_CHANNEL_TEMP, SAMPLE_TIME_TEMP);
	adc_set_sample_time(ADC1, ADC_CHANNEL_VREF, SAMPLE_TIME_VREF);
#else
	adc_set_sample_time_on_all_channels(ADC1, SAMPLE_TIME_BASIC);
#endif
	adc_enable_temperature_sensor();
#if (SEPARATE_VREF == 1)
	adc_enable_vrefint();
#endif

	adc_enable_scan_mode(ADC1);
	uint8_t channel_array[] = {EXT_CHAN_CHAN, ADC_CHANNEL_TEMP, ADC_CHANNEL_VREF};
        adc_set_regular_sequence(ADC1, ARRAY_SIZE(channel_array), channel_array);

        adc_enable_external_trigger_regular(ADC1, ADC_CR2_EXTSEL_TIM6_TRGO, ADC_CR2_EXTEN_RISING_EDGE);
        /* power down when not in use please. */
        ADC_CR1(ADC1) |= ADC_CR1_PDD | ADC_CR1_PDI;
        adc_power_on(ADC1);

#if 1
	/* adc-v2 has this as "adc_is_power_on*/
        while ((ADC_SR(ADC1) & ADC_SR_ADONS) == 0) {
                ;
        }
#endif

	/* hard code a threshold for now , around a third */
	adc_set_watchdog_low_threshold(ADC1, 1400);
	adc_enable_analog_watchdog_on_selected_channel(ADC1, EXT_CHAN_CHAN);
	adc_enable_analog_watchdog_regular(ADC1);
	adc_enable_awd_interrupt(ADC1);
	nvic_enable_irq(NVIC_ADC1_IRQ);

	dma_channel_reset(DMA1, DMA_CHANNEL1); // channel1 is adc
	dma_set_memory_address(DMA1, DMA_CHANNEL1, (uint32_t)adc_buffer);
	dma_set_memory_size(DMA1, DMA_CHANNEL1, DMA_CCR_MSIZE_16BIT);
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL1);
	dma_set_peripheral_address(DMA1, DMA_CHANNEL1, (uint32_t) & ADC_DR(ADC1));
	dma_set_peripheral_size(DMA1, DMA_CHANNEL1, DMA_CCR_PSIZE_16BIT);
	dma_set_number_of_data(DMA1, DMA_CHANNEL1, ARRAY_SIZE(adc_buffer));

//	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL1);
	dma_enable_transfer_error_interrupt(DMA1, DMA_CHANNEL1);
	dma_set_read_from_peripheral(DMA1, DMA_CHANNEL1);

	// This is essential to stop overflows.
	dma_enable_circular_mode(DMA1, DMA_CHANNEL1);

	dma_enable_channel(DMA1, DMA_CHANNEL1);
	nvic_enable_irq(NVIC_DMA1_CHANNEL1_IRQ);

	//Keep on requesting DMA, as long as ADC is running
	ADC_CR2(ADC1) |= ADC_CR2_DDS;
	adc_enable_dma(ADC1);

	timer_trigger_setup(100);  // slowly for now...
	/* Reset timer count and start */
        TIM_CNT(TIM6) = 0;
        timer_enable_counter(TIM6);

}

static float adc_calc_tempf(unsigned int adc_raw_temp) {
#if (HAS_ROM_CALIBRATION == 1)
	float slope = ((110-30) * 1.0f)/((ST_TSENSE_CAL2_110C - ST_TSENSE_CAL1_30C) * 1.0f);
	return slope * (adc_raw_temp - ST_TSENSE_CAL1_30C) + 30;
#else
	return ts_v * 1.0;
#endif
}

void task_run(void)
{
	static int last_awd_hits;
	float temperature = adc_calc_tempf(adc_buffer[1]);
	printf("Last adc reading: %d, last temp: %f (raw=%d), last cal: %d\n",
		adc_buffer[0], temperature, adc_buffer[1], adc_buffer[2]);
	if (awd_hits > last_awd_hits) {
		printf("Got %d AWD hits since last run\n", awd_hits);
		last_awd_hits = awd_hits;
	}
}
