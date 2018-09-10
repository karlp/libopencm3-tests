/**
 * CMSIS-RTOS2 OS Tick implementation for libopencm3
 * We aren't using CMSIS itself, so we can't use their
 * standard built in one.
 */

#include "os_tick.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>

int32_t OS_Tick_Setup (uint32_t freq, IRQHandler_t handler)
{
	(void)handler;
	
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	uint32_t load = rcc_ahb_frequency / freq;
	if (load > 0xffffff) {
		/* as per implementation */
		return -1;
	}
	systick_set_reload(load);
	/* possibly fiddle with irq priorities here */
	systick_interrupt_enable();
	return 0;
}

void OS_Tick_Enable (void)
{
	systick_counter_enable();
}

void OS_Tick_Disable (void)
{
	systick_counter_disable();
}

void OS_Tick_AcknowledgeIRQ (void)
{
	(void)STK_CSR;
}

int32_t  OS_Tick_GetIRQn (void)
{
	return -1;
}

uint32_t OS_Tick_GetClock (void)
{
	return rcc_ahb_frequency;
}

uint32_t OS_Tick_GetInterval (void)
{
	return systick_get_reload();
}

uint32_t OS_Tick_GetCount (void)
{
	return systick_get_reload() - systick_get_value();
}

uint32_t OS_Tick_GetOverflow (void)
{
	return systick_get_countflag();
}

/* also, include our locm3 handler relays here */

extern void SysTick_Handler(void);
extern void PendSV_Handler(void);
extern void SVC_Handler(void);

void sys_tick_handler(void) {
	SysTick_Handler();
}
void pend_sv_handler(void) {
	PendSV_Handler();
}
void sv_call_handler(void) {
	SVC_Handler();
}
