
PROJECT	    = rtos2-gadget-zero-hw1

SHARED_DIR = ../../shared $(OPENCM3_DIR)/tests/gadget-zero
CFILES = $(PROJECT).c
CFILES += usb-gadget0.c trace.c trace_stdio.c
CFILES += delay.c

VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))


CMSIS_ROOT=/home/karlp/src/oocd-rtos-locm3/cmsis-rtos-rtx/CMSIS_5
# make all of the below a rtos2.rules.mk thing?
RTOS2_DIR = $(CMSIS_ROOT)/CMSIS/RTOS2
RTOS2_INCS = $(RTOS2_DIR)/RTX/Include $(RTOS2_DIR)/Include
# We can't use cmsis's systick with libopencm3, as it expects all the cmsis headers
#RTOS2_SRC = $(RTOS2_DIR)/Source/os_systick.c
RTOS2_SRCS = os_systick_locm3.c
RTOS2_SRCS += rtx_config.c
RTOS2_SRCS += $(RTOS2_DIR)/RTX/Source/GCC/irq_cm3.S
RTOS2_SRCS += rtx_delay.c rtx_evflags.c rtx_evr.c rtx_kernel.c
RTOS2_SRCS += rtx_lib.c rtx_memory.c rtx_mempool.c rtx_msgqueue.c
RTOS2_SRCS += rtx_mutex.c rtx_semaphore.c rtx_system.c
RTOS2_SRCS += rtx_thread.c rtx_timer.c

VPATH += $(RTOS2_DIR)/RTX/Source $(RTOS2_DIR)/RTX/Source/GCC

CMSIS_INCS = $(CMSIS_ROOT)/CMSIS/Core/Include $(CMSIS_ROOT)/Device/ARM/ARMCM3/Include
# Inclusion of header files
INCLUDES += $(patsubst %,-I%, . $(RTOS2_INCS) $(CMSIS_INCS))
CFILES += $(RTOS2_SRCS)
#  down to here...

OPENCM3_DIR=../../libopencm3
DEVICE=stm32l151xb
OOCD_FILE = openocd.stm32l1-generic.cfg
include ../../rules.mk

