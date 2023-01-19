
#include <stdint.h>
#include <stdio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/dma.h>

#include "funcgen.h"
#include "trace.h"

static void *dma_tc_user_data;
struct plat_data {
	void *dma_tc_user_data;
	void(*cb_dma_tc)(void *user_data);
};

struct plat_data pdata;

/* FIXME - move this to a layer of platform code, with "null", "f4" and "l1" platforms */
void funcgen_plat_dma_setup(int channel, const uint32_t *wave_table, int wave_table_count,
	void(*cb_dma_tc)(void *user_data), void *user_data)
{
	/* DAC channel 1 (tim6) uses DMA channel 2. */
	/* DAC channel 2 (tim7) uses DMA channel 3. */
	uint8_t dchan;
	uint8_t dchan_nvic;
	uint32_t daddr;

	dma_channel_reset(DMA1, dchan);
	printf("setting DMA (%u) from %#x of len: %d\n", dchan, (unsigned int)wave_table, wave_table_count);
	dma_set_memory_address(DMA1, dchan, (uint32_t) wave_table);
	/* Our wave table is 16+16 duplicated, so we can do phase coherant dac1+2 for the same data,
	 * so we are always 32bit memory size, but we want to only copy 16bits in single mode... */
	dma_set_memory_size(DMA1, dchan, DMA_CCR_MSIZE_32BIT);
		dma_set_peripheral_size(DMA1, dchan, DMA_CCR_PSIZE_16BIT);
	dma_enable_memory_increment_mode(DMA1, dchan);
	dma_set_read_from_memory(DMA1, dchan);
	dma_set_peripheral_address(DMA1, dchan, daddr);
	dma_set_number_of_data(DMA1, dchan, wave_table_count);
	dma_enable_circular_mode(DMA1, dchan);

	if (cb_dma_tc) {
		if (user_data) {
			pdata.dma_tc_user_data = user_data;
		}
		pdata.cb_dma_tc = cb_dma_tc;
		nvic_enable_irq(dchan_nvic);
		dma_enable_transfer_complete_interrupt(DMA1, dchan);
	}

	dma_enable_channel(DMA1, dchan);

}

void dma1_channel2_isr(void) {
	if (dma_get_interrupt_flag(DMA1, DMA_CHANNEL2, DMA_TCIF)) {
		if (pdata.cb_dma_tc) {
			trace_send_blocking8(23, 'd');
			pdata.cb_dma_tc(pdata.dma_tc_user_data);
		}
		dma_clear_interrupt_flags(DMA1, DMA_CHANNEL2, DMA_TCIF);
	}
}

void dma1_channel3_isr(void) {
	if (dma_get_interrupt_flag(DMA1, DMA_CHANNEL3, DMA_TCIF)) {
		if (pdata.cb_dma_tc) {
			trace_send_blocking8(23, 'd');
			pdata.cb_dma_tc(pdata.dma_tc_user_data);
		}
		dma_clear_interrupt_flags(DMA1, DMA_CHANNEL3, DMA_TCIF);
	}
}
