
PROJECT	    = host-usb-spi-freertos
SHARED_DIR = ../../../shared ../
CFILES = host-usb-spi.c
CFILES += trace.c trace_stdio.c

VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))

FREERTOS_DIR = ../../../FreeRTOSv10.1.0/FreeRTOS
include ../../../freertos.rules.mk

OPENCM3_DIR=../../../libopencm3
DEVICE=stm32l151xb
OOCD_FILE = openocd.stm32l1-generic.cfg
include ../../../rules.mk


