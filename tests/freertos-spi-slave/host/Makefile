
PROJECT	    = host-usb-spi-freertos
SHARED_DIR = ../../../shared ../
CFILES = host-usb-spi.c
CFILES += trace.c trace_stdio.c

VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))

FREERTOS_DIR = ../../../../FreeRTOSv10.1.1/FreeRTOS
include ../../../freertos.cm3.rules.mk

OPENCM3_DIR=../../../libopencm3
DEVICE=stm32l151xb
OOCD_FILE = ../../../openocd/openocd.host-stm32l1.cfg
include ../../../rules.mk


