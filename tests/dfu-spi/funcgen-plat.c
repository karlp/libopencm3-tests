#include <stdio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include "funcgen.h"


void funcgen_plat_timer_setup(int channel, int period_ns) {
	uint32_t timer;
	uint32_t timer_rst;
	switch (channel) {
	case 1:
		timer = TIM7;
		timer_rst = RST_TIM7;
		break;
	case 2:
	case 0:
	default:
		timer = TIM6;
		timer_rst = RST_TIM6;
		break;
	}
	// THIS IS L1 SPECIFIC NOW
	rcc_periph_reset_pulse(timer_rst);
	// original guidelines when we left off this on an f4 was...
	// target ~50nS steps at bottom end?
	// dac says 1msps max max, so at 1msps, we want a period of what, 1 Mhz _overflows_
	// so at least 2 Mhz clock..., let's say 4 Mhz timer clock for max res stuff
	if (period_ns > 50) {
		timer_set_prescaler(timer, 0); // timers are at 32MHz,
		timer_set_period(timer, (period_ns / 32) - 1); // so this is good enough.
	}
//	if (period_ns * 50 > 0x6000) {
//		/* don't even try and run that fast with this slow a wave */
//		timer_set_prescaler(timer, 83); // 1Mhz (84/1 - 1) ticks at 1usecs
//		timer_set_period(timer, (period_ns / 1000) - 1);
//	}

	timer_enable_irq(timer, TIM_DIER_UIE);
	timer_set_master_mode(timer, TIM_CR2_MMS_UPDATE);
	timer_enable_counter(timer);
}


void funcgen_plat_dac_setup(int channel) {
	/* Setup the DAC channel 1, with timer 2 as trigger source.
	 * Assume the DAC has woken up by the time the first transfer occurs */
	int chan;
	switch (channel) {
	case 2:
		chan = DAC_CHANNEL_BOTH;
		dac_set_trigger_source(DAC1,DAC_CR_TSEL1_T6 | DAC_CR_TSEL2_T6);
		break;
	case 1:
		dac_set_trigger_source(DAC1, DAC_CR_TSEL2_T7);
		chan = DAC_CHANNEL2;
		break;
	default:
	case 0:
		dac_set_trigger_source(DAC1, DAC_CR_TSEL1_T6);
		chan = DAC_CHANNEL1;
		break;
	}
}

#if 0
void TIMER_DAC1_ISR(void) {
	timer_clear_flag(TIM6, TIM_SR_UIF);
	gpio_toggle(GPIOC, GPIO1);
}
#endif


void funcgen_plat_output(int channel, bool enable) {
}

void funcgen_plat_init(void) {
	rcc_periph_clock_enable(RCC_DMA1);
	rcc_periph_clock_enable(RCC_DAC);
	rcc_periph_clock_enable(RCC_GPIOA); /* DAC output pins */
	rcc_periph_clock_enable(RCC_TIM6); /* channel 1 triggers */
	rcc_periph_clock_enable(RCC_TIM7); /* channel 2 triggers */
	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO4 | GPIO5);

	/* timing tests */
	//rcc_periph_clock_enable(RCC_GPIOC);
	//gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1);

	/* Just enable both permanently in NVIC */
	//nvic_enable_irq(TIMER_DAC1_NVIC); /* only used for timing debug */
	/* nvic_enable_irq(NVIC_TIM7_IRQ); */
	
	/* hack to auto start */
	//funcgen_sin(0, 1000, FULL_SCALE / 2, FULL_SCALE / 2);
}

void funcgen_plat_buffer(int channel, bool enable)
{
}
