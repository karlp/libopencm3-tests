/*
 * support for stdio output to a usart
 * Karl Palsson, 2015 <karlp@tweak.net.au>
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <libopencm3/stm32/usart.h>

#ifndef STDIO_USART
#define STDIO_USART USART1
#endif

int _write(int file, char *ptr, int len);
int _write(int file, char *ptr, int len)
{
	int i;

	if (file == STDOUT_FILENO || file == STDERR_FILENO) {
		for (i = 0; i < len; i++) {
			if (ptr[i] == '\n') {
				usart_send_blocking(STDIO_USART, '\r');
			}
			usart_send_blocking(STDIO_USART, ptr[i]);
		}
		return i;
	}
	errno = EIO;
	return -1;
}


