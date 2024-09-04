# Pedro Botelho

# Project Variables

CPU 			:= cortex-m0plus
LINKER_SCRIPT 	:= bsp/linker_script.lds
SPECS 			:= nano.specs
HOME			:= $(shell whoami)
BOARD			:= FRDM-KL46Z
UPLOAD_PATH		:= /media/${HOME}/${BOARD}/
OUTPUT			:= build/firmware
OUTPUT_ELF		:= ${OUTPUT}.elf
OUTPUT_BIN		:= ${OUTPUT}.bin

TC				:= arm-none-eabi
CC				:= ${TC}-gcc
AS				:= ${TC}-as
OBJCPY			:= ${TC}-objcopy
OBJDMP			:= ${TC}-objdump

# Compile Fla

AFLAGS  := -mcpu=${CPU} -mthumb
CFLAGS  := -mcpu=${CPU} -mthumb --specs=${SPECS} -nostdlib "-T${LINKER_SCRIPT}" -Wall 
LDFLAGS := ${CFLAGS} "-Wl,-Map,build/firmware.map" -nostartfiles

# Makefile Begin

.PHONY: build upload disassembly clean

all: build upload

# Commands

build: ${OUTPUT_BIN}

upload: ${OUTPUT_BIN}
	cp $< ${UPLOAD_PATH}

disassembly: build/firmware.s

clean:
	rm -rf build/*

# Regular Targets

${OUTPUT_ELF}: build/main.o bsp/linker_script.lds
	${CC} build/main.o -o $@ $(LDFLAGS)

build/main.o: src/main.c
	${CC} $(CFLAGS) -c $< -o $@
	
${OUTPUT_BIN}: ${OUTPUT_ELF}
	${OBJCPY} -S -O binary $< $@

build/firmware.s: ${OUTPUT_ELF}
	${OBJDMP} -D -S $< > $@