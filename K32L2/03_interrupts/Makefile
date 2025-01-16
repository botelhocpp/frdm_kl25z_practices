# Pedro Botelho

# Project Variables

CPU 			:= cortex-m0plus
LINKER_SCRIPT 	:= bsp/linker_script.lds
SPECS 			:= nano.specs
HOME			:= $(shell whoami)
BOARD			:= FRDM-K32L2B/
UPLOAD_PATH		:= /media/${HOME}/${BOARD}/
OUTPUT			:= build/firmware
OUTPUT_ELF		:= ${OUTPUT}.elf
OUTPUT_BIN		:= ${OUTPUT}.bin

TC				:= arm-none-eabi
CC				:= ${TC}-gcc
AS				:= ${TC}-as
GDB				:= ${TC}-gdb
OBJCPY			:= ${TC}-objcopy
OBJDMP			:= ${TC}-objdump

# Compile Flags

AFLAGS  := -g -mcpu=${CPU} -mthumb
CFLAGS  := -Os -Ibsp -g -mcpu=${CPU} -mthumb --specs=${SPECS} -nostdlib "-T${LINKER_SCRIPT}" -Wall 
LDFLAGS := ${CFLAGS} "-Wl,-Map,build/firmware.map" -nostartfiles

# Makefile Begin

.PHONY: build upload disassembly dump clean debug

all: build upload

# Commands

build: ${OUTPUT_BIN}

upload: ${OUTPUT_BIN}
	# cp $< ${UPLOAD_PATH} # Drag-and-drop
	openocd -f bsp/tcl/interface/cmsis-dap.cfg -f bsp/tcl/board/k32.cfg -c "program $< verify reset exit"

disassembly: build/firmware.s

clean:
	rm -rf build/*

# Regular Targets

${OUTPUT_ELF}: build/start.o build/main.o bsp/linker_script.lds
	${CC} build/start.o build/main.o -o $@ $(LDFLAGS)

build/main.o: src/main.c
	${CC} $(CFLAGS) -c $< -o $@

build/start.o: src/start.s
	${AS} $(AFLAGS) -c $< -o $@
	
${OUTPUT_BIN}: ${OUTPUT_ELF}
	${OBJCPY} -S -O binary $< $@

build/firmware.s: ${OUTPUT_ELF}
	${OBJDMP} -D -S $< > $@

dump: build/dump.txt

build/dump.txt: ${OUTPUT_ELF}
	${OBJDMP} -D $< > $@

debug: ${OUTPUT_ELF}
	${GDB} $<
