/**
 * Null implementations of required portsions
 * These are often inside "RTX_Config.c"
 */
#include <stdint.h>
#include <stdio.h>

#include "rtx_os.h"

#ifdef ER_DEBUG
#define ER_DPRINTF(fmt, ...) \
	do { printf(fmt, ## __VA_ARGS__); } while (0)
#else
#define ER_DPRINTF(fmt, ...) \
	do { } while (0)
#endif

void osRtxIdleThread (void *argument)
{
	(void)argument;
	while (1)  {
		;
	}
}

uint32_t osRtxErrorNotify (uint32_t code, void *object_id)
{
	/* Like you're going to look at it anyway */
	(void)object_id;
	switch (code) {
	case osRtxErrorStackUnderflow: ER_DPRINTF("!!!STACK UNDERFLOW\n"); break;
	case osRtxErrorISRQueueOverflow: ER_DPRINTF("!!!ISR_Q Overflow\n"); break;
	case osRtxErrorTimerQueueOverflow: ER_DPRINTF("!!!Timer_Q Overflow\n"); break;
	case osRtxErrorClibSpace: ER_DPRINTF("!!!ClibSpace\n"); break;
	case osRtxErrorClibMutex: ER_DPRINTF("!!!ClibMutex\n"); break;
	default: ER_DPRINTF("!!!Unknown error code?!\n"); break;
	}
	while (1) {
		;
	}
}
