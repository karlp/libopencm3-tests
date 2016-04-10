##
## This file is part of the libopencm3 project.
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##

BOARD = lm3s
PROJECT = qemu-$(BOARD)
BUILD_DIR = bin-$(BOARD)

SHARED_DIR = ../shared

CFILES = main.c

VPATH += $(SHARED_DIR)

INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))

OPENCM3_DIR=../../libopencm3

DEVICE=lm3s6965

include ../../rules.mk

qemu: $(PROJECT).elf
	qemu-system-arm -semihosting -M lm3s6965evb --kernel $^ -serial stdio

qemu-dbg: $(PROJECT).elf
	qemu-system-arm -semihosting -M lm3s6965evb --kernel $^ -serial stdio -S -gdb tcp::3333
