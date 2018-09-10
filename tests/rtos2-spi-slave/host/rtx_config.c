/**
 * Null implementations of required portsions
 * These are often inside "RTX_Config.c"
 */
#include <stdint.h>

#include "rtx_os.h"

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
	(void)code;
	(void)object_id;
	while (1) {
		;
	}
}

