/*
 * support for stdio output to a trace port
 * Karl Palsson, 2014 <karlp@remake.is>
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "trace.h"

int _write(int file, char *ptr, int len);
/* trace channel 0 is always debug printf by convention */
int _write(int file, char *ptr, int len)
{
	int i;

	if (file == STDOUT_FILENO || file == STDERR_FILENO) {
		for (i = 0; i < len; i++) {
			if (ptr[i] == '\n') {
				trace_send_blocking8(0, '\r');
			}
			trace_send_blocking8(0, ptr[i]);
		}
		return i;
	}
	errno = EIO;
	return -1;
}


