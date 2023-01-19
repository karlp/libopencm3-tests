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


#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif
