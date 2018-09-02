
PROJECT	    = freertos-gadget-zero
SHARED_DIR = ../../shared $(OPENCM3_DIR)/tests/gadget-zero
CFILES = freertos-gadget-zero-hw1.c
CFILES += usb-gadget0.c trace.c trace_stdio.c
CFILES += delay.c

VPATH += $(SHARED_DIR) 
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))

FREERTOS_DIR = ../../FreeRTOSv10.1.0/FreeRTOS
#FREERTOS_DIR = /home/karlp/src/FreeRTOSV8.2.1/FreeRTOS
FREERTOS_PORT = $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM3
FREERTOS_INC = $(FREERTOS_DIR)/Source/include
FREERTOS_SRC = $(FREERTOS_DIR)/Source
FREERTOS_MMG = $(FREERTOS_DIR)/Source/portable/MemMang
FREERTOS_SRCS = list.c queue.c tasks.c port.c heap_1.c
FREERTOS_SRCS += croutine.c timers.c
# FIXME - check if still needed in freertos 10?
#FREERTOS_SRCS += FreeRTOS-openocd.c
CFILES += $(FREERTOS_SRCS)

VPATH += $(FREERTOS_SRC) $(FREERTOS_PORT) $(FREERTOS_MMG)
INCLUDES += $(patsubst %,-I%, . $(FREERTOS_INC) $(FREERTOS_PORT))

#LDFLAGS += -Wl,--undefined=uxTopUsedPriority
#LDFLAGS += -Wl,--undefined=xDelayedTaskList1

OPENCM3_DIR=../../libopencm3
DEVICE=stm32l151xb
OOCD_FILE = openocd.stm32l1-generic.cfg
include ../../rules.mk


