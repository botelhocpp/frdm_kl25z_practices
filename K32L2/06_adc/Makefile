# Pedro Botelho

# Project Variables

CPU 			:= cortex-m0plus
LINKER_SCRIPT 	:= bsp/linker_script.lds
SPECS 			:= nosys.specs
HOME			:= $(shell whoami)
BOARD			:= FRDM-K32L2B/
UPLOAD_PATH		:= /media/${HOME}/${BOARD}/
OUTPUT			:= build/firmware
OUTPUT_ELF		:= ${OUTPUT}.elf
OUTPUT_BIN		:= ${OUTPUT}.bin
LIB_PATH 		:= ~/.platformio/packages/toolchain-gccarmnoneeabi/lib/gcc/arm-none-eabi/9.3.1/

TC				:= arm-none-eabi
CC				:= ${TC}-gcc
AS				:= ${TC}-as
GDB				:= ${TC}-gdb
OBJCPY			:= ${TC}-objcopy
OBJDMP			:= ${TC}-objdump

# Compile Flags

AFLAGS  := -g -mcpu=${CPU} -mthumb
CFLAGS  := -Os -Ibsp -g -mcpu=${CPU} -mthumb --specs=${SPECS} "-T${LINKER_SCRIPT}" -Wall 
LDFLAGS := ${CFLAGS} "-Wl,-Map,build/firmware.map"

# Makefile Begin

.PHONY: build upload disassembly dump clean debug openocd telnet

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
	${CC} build/start.o build/main.o -o $@ $(LDFLAGS) -L${LIB_PATH} -lgcc -lc -lm

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

openocd:
	openocd -f bsp/tcl/interface/cmsis-dap.cfg -f bsp/tcl/board/k32.cfg

debug: ${OUTPUT_ELF}
	${GDB} -ex "target remote localhost:3333" -ex "monitor reset halt" -ex "break main" -ex "load" -ex "continue" -ex "layout src" $<
