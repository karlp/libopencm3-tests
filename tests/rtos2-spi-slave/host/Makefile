
PROJECT	    = host-usb-spi-rtos2
SHARED_DIR = ../../../shared ../
CFILES = host-usb-spi.c
CFILES += trace.c trace_stdio.c

VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))

CMSIS_ROOT=/home/karlp/src/oocd-rtos-locm3/cmsis-rtos-rtx/CMSIS_5
include ../../../cmsis-rtos2.rules.mk
CFILES += rtx_config.c

OPENCM3_DIR=../../../libopencm3
DEVICE=stm32l151xb
OOCD_FILE = openocd.stm32l1-generic.cfg
include ../../../rules.mk


