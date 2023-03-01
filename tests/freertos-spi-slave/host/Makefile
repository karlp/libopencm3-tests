
PROJECT	    = host-usb-spi-freertos
SHARED_DIR = ../../../shared ../
CFILES = host-usb-spi.c dfu-impl.c
CFILES += trace.c trace_stdio.c

VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))

FREERTOS_DIR = ../../../../FreeRTOS-KernelV10.5.1
include ../../../freertos.cm3.rules.mk

OPENCM3_DIR=../../../libopencm3
DEVICE=stm32l151xb
OOCD_FILE = ../../../openocd/openocd.host-stm32l1.cfg
include ../../../rules.mk

LDSCRIPT=app.ld

