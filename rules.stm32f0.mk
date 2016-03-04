LDSCRIPT = $(OPENCM3_DIR)/lib/stm32/f0/stm32f07xzb.ld
OPENCM3_LIB = opencm3_stm32f0
OPENCM3_DEFS = -DSTM32F0
#FP_FLAGS ?= -mfloat-abi=hard -mfpu=fpv4-sp-d16
ARCH_FLAGS = -mthumb -mcpu=cortex-m0 $(FP_FLAGS)
#OOCD_INTERFACE = stlink-v2
#OOCD_TARGET = stm32f4x
OOCD_FILE = ../../openocd/openocd.stm32f072-disco.cfg

