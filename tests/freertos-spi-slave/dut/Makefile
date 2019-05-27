
PROJECT	    = dut-spi-slave-freertos
SHARED_DIR = ../../../shared ../
CFILES = dut-spi-slave.c
# No trace on cm0
CFILES += trace.c trace_stdio.c

VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))

FREERTOS_DIR = ../../../../FreeRTOSv10.1.1/FreeRTOS
#include ../../../freertos.cm0.rules.mk
include ../../../freertos.cm3.rules.mk

OPENCM3_DIR=../../../libopencm3
#DEVICE=stm32l072rz
DEVICE=stm32l152re
#OOCD_FILE = openocd.stm32l0-generic.cfg
OOCD_FILE = openocd.stm32l1-generic.cfg
include ../../../rules.mk


