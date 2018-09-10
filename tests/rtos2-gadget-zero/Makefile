PROJECT	    = rtos2-gadget-zero-hw1

SHARED_DIR = ../../shared $(OPENCM3_DIR)/tests/gadget-zero
CFILES = $(PROJECT).c
CFILES += usb-gadget0.c trace.c trace_stdio.c
CFILES += delay.c

VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))

CMSIS_ROOT=/home/karlp/src/oocd-rtos-locm3/cmsis-rtos-rtx/CMSIS_5
include ../../cmsis-rtos2.rules.mk
CFILES += rtx_config.c

OPENCM3_DIR=../../libopencm3
DEVICE=stm32l151xb
OOCD_FILE = openocd.stm32l1-generic.cfg
include ../../rules.mk
