
#FREERTOS_DIR = ../../../FreeRTOSv10.1.0/FreeRTOS/Source
#FREERTOS_DIR = /home/karlp/src/FreeRTOSV8.2.1/FreeRTOS/Source
FREERTOS_PORT = $(FREERTOS_DIR)/portable/GCC/ARM_CM3
FREERTOS_INC = $(FREERTOS_DIR)/include
FREERTOS_MMG = $(FREERTOS_DIR)/portable/MemMang
FREERTOS_SRCS = list.c queue.c tasks.c port.c heap_4.c
FREERTOS_SRCS += croutine.c timers.c
# Only required for freertos 7.5.3 -> 10.4.1 inclusive
#FREERTOS_SRCS += FreeRTOS-openocd.c
CFILES += $(FREERTOS_SRCS)

VPATH += $(FREERTOS_DIR) $(FREERTOS_PORT) $(FREERTOS_MMG)
INCLUDES += $(patsubst %,-I%, . $(FREERTOS_INC) $(FREERTOS_PORT))

LDFLAGS += -Wl,--undefined=uxTopUsedPriority
#LDFLAGS += -Wl,--undefined=xDelayedTaskList1

