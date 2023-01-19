#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "funcgen.h"
//#include "sync.h"
#include "syscfg.h"

static struct funcgen_output_t _output1 = {
	.enabled = false,
	.ampl = 0.1,
	.offset = FULL_SCALE / 2,
	.freq = 1000,
};

static struct funcgen_output_t _output2 = {
	.enabled = false,
	.ampl = 0.1,
	.offset = FULL_SCALE / 2,
	.freq = 1000,
};

static struct funcgen_state_t state = {
	.outputs = { &_output1, &_output2, &_output1 },
};

/* http://yehar.com/blog/?p=1220 perhaps */
/* From ST's example code */
const uint16_t lut_sine[] = {
	2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
	3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
	599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647, 2047
};

// TODO - need to extract common ch1/ch2 code...


/*
 * I ARE DUM
 * http://stackoverflow.com/questions/3304513/stretching-out-an-array
 */
static float interp1(float x, const uint16_t a[], int n)
{
	if (x <= 0) {
		return a[0];
	}
	if (x >= n - 1) {
		return a[n - 1];
	}
	int j = x;
	return a[j] + (x - j) * (a[j + 1] - a[j]);
}

static void calculate_output(const uint16_t *source, int source_len,
	uint32_t *dest, int dest_len,
	float ampl, float offset)
{
	float nom = source_len - 1;
	float denom = dest_len - 1;
	float step = nom / denom;
	float offset_bits = (offset / FULL_SCALE) * 4095;
	/* remember, our waveforms are 0..x, so we need to subtract half our
	 * ampl to get the offset _centered_*/
	float ampl_bits = (ampl / FULL_SCALE) * 4095;
	offset_bits -= ampl_bits / 2;
	for (int i = 0; i < dest_len; i++) {
		float si = interp1(i*step, source, source_len);
		/* We're filling the 32bit dest buffer with duplicate for dual dac mode
		 * but we'll only read one of them via DMA in single mode
		 */
		uint16_t value = si * (ampl / FULL_SCALE) + offset_bits;
		dest[i] = value << 16 | value;
	}
}

static void my_dma_cb(void *udata)
{
	struct bar_faker_app_state *st = udata;
	//st->last_zx = sync_read_timer();
}

void funcgen_sin(int channel, float frequency, float ampl, float offset) {
	uint32_t *wavedata = state.outputs[channel]->waveform;
	int dest_len = ARRAY_LENGTH(state.outputs[channel]->waveform);
	
	/* Take the input wave and calculate the wavetable for DMA */
	calculate_output(lut_sine, ARRAY_LENGTH(lut_sine), wavedata, dest_len, ampl, offset);
	//calculate_output(lut_sine, 32, wavedata, FUNCGEN_WAVE_SIZE, ampl, offset);

	float usecs_per_wave = 1000000 / frequency;
	int nanos_per_sample = ceilf(1000 * usecs_per_wave / (dest_len * 1.0f));
	printf("Requested freq: %f, usecs/wave: %f, timer nsecs: %d\n", frequency, usecs_per_wave, nanos_per_sample);
	
	/*+++ hardware setup +++*/
	funcgen_plat_timer_setup(channel, nanos_per_sample);
	funcgen_plat_dma_setup(channel, wavedata, dest_len, my_dma_cb, state.user_data);
	funcgen_plat_dac_setup(channel);
	/*++++++++++++++++++++++*/
	
        state.outputs[channel]->mode = OUTPUT_MODE_SINE;
	state.outputs[channel]->enabled = true;
	state.outputs[channel]->freq = frequency;
	state.outputs[channel]->ampl = ampl;
	state.outputs[channel]->offset = offset;
	/* we're not doing any tricks on variable lengths for better frequency control at the moment */
	state.outputs[channel]->waveform_length = dest_len;
}

void funcgen_user(int channel, float frequency, float ampl, float offset) {
        uint32_t *wavedata = state.outputs[channel]->waveform;
	int dest_len = state.outputs[channel]->waveform_length;

	float usecs_per_wave = 1000000 / frequency;
	int nanos_per_sample = ceil(1000 * usecs_per_wave / (dest_len * 1.0));
	printf("Requested freq: %f, usecs/wave: %f, timer nsecs: %d\n", frequency, usecs_per_wave, nanos_per_sample);

	/*+++ hardware setup +++*/
	funcgen_plat_timer_setup(channel, nanos_per_sample);
	funcgen_plat_dma_setup(channel, wavedata, dest_len, my_dma_cb, state.user_data);
	funcgen_plat_dac_setup(channel);
	/*++++++++++++++++++++++*/

        state.outputs[channel]->mode = OUTPUT_MODE_USER;
	state.outputs[channel]->enabled = true;
	state.outputs[channel]->freq = frequency;
	state.outputs[channel]->ampl = ampl;
	state.outputs[channel]->offset = offset;
	/* we're not doing any tricks on variable lengths for better frequency control at the moment */
	state.outputs[channel]->waveform_length = dest_len;
}

void funcgen_output(int channel, bool enable)
{
	printf("setting channel %d to %s\n", channel, enable ? "ON" : "off");
	funcgen_plat_output(channel, enable);
	state.outputs[channel]->enabled = enable;
}

void funcgen_buffer(int channel, bool enable)
{
	funcgen_plat_buffer(channel, enable);
	// FIXME should be in the state object too really.
}


struct funcgen_state_t* funcgen_getstate(void) {
	return &state;
}

void funcgen_init(void *user_data)
{
	if (user_data) {
		state.user_data = user_data;
	}
	funcgen_plat_init();
}
