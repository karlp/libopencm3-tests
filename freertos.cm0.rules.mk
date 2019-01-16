
#FREERTOS_DIR = ../../../FreeRTOSv10.1.0/FreeRTOS
#FREERTOS_DIR = /home/karlp/src/FreeRTOSV8.2.1/FreeRTOS
FREERTOS_PORT = $(FREERTOS_DIR)/Source/portable/GCC/ARM_CM0
FREERTOS_INC = $(FREERTOS_DIR)/Source/include
FREERTOS_SRC = $(FREERTOS_DIR)/Source
FREERTOS_MMG = $(FREERTOS_DIR)/Source/portable/MemMang
FREERTOS_SRCS = list.c queue.c tasks.c port.c heap_1.c
FREERTOS_SRCS += croutine.c timers.c
# FIXME - check if still needed in freertos 10?
FREERTOS_SRCS += FreeRTOS-openocd.c
CFILES += $(FREERTOS_SRCS)

VPATH += $(FREERTOS_SRC) $(FREERTOS_PORT) $(FREERTOS_MMG)
INCLUDES += $(patsubst %,-I%, . $(FREERTOS_INC) $(FREERTOS_PORT))

LDFLAGS += -Wl,--undefined=uxTopUsedPriority
#LDFLAGS += -Wl,--undefined=xDelayedTaskList1

