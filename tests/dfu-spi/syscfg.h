/*
 * Shared definitions for the system as a whole
 */

#pragma once

#define IRQ2NVIC_PRIOR(x)	((x)<<4)

#define ER_DEBUG
#ifdef ER_DEBUG
#define ER_DPRINTF(fmt, ...) \
    do { printf(fmt, ## __VA_ARGS__); } while (0)
#else
#define ER_DPRINTF(fmt, ...) \
    do { } while (0)
#endif


struct bar_faker_app_state {
	/* this is the internal timestamp of the last DAC window start */
	uint32_t last_zx;
	/* this is experimentally determined to give us pf 1.0 as a starting point */
	int32_t base_offset;
	/* This is added to the last_zx to generate the "reported" zero crossing */
	int32_t zx_phase_adjust[3];
};


#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif
