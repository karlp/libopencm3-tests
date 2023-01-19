/*
 * STM32L151C8-A, 64K flash total, 8k for bootloader
 */

/* Define memory regions. */
MEMORY
{
	rom (rx) : ORIGIN = 0x08002000, LENGTH = 56K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 32K
	eep (r) : ORIGIN = 0x08080000, LENGTH = 4K
}

INCLUDE ../../libopencm3/lib/cortex-m-generic.ld


