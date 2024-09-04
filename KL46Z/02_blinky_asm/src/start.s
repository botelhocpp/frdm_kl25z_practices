.syntax unified

/* SIM */
.equ SIM_ADDR, 	    (0x40047000u)
.equ SIM_SCGC5, 	(0x1038u)
.equ SIM_COPC, 	    (0x1100u)

/* PORT */
.equ PORTB_ADDR, 	(0x4004A000u)
.equ PORTD_ADDR, 	(0x4004C000u)
.equ PORT_PCR, 	    (0x0004u)

/* GPIO */
.equ GPIOB_ADDR, 	(0x400FF040u)
.equ GPIOD_ADDR, 	(0x400FF0C0u)
.equ GPIO_PSOR, 	(0x0004u)
.equ GPIO_PCOR, 	(0x0008u)
.equ GPIO_PDDR, 	(0x0014u)


# Vector table
.section .vectors, "a"
    .align 2
    .long  _estack
    .long  _start

# Vector table
.section .cfmconfig, "a"
    .long 0xFFFFFFFF
    .long 0xFFFFFFFF
    .long 0xFFFFFFFF
    .long 0xFFFFFFFE

.section .text
.thumb_func
.align 2
.global _start
_start:
    cpsid i

    eors r0, r0, r0

    # Disable Watchdogs
    ldr r1, =(SIM_ADDR + SIM_COPC)
    str r0, [r1] 

    # Zero .bss
    ldr r1, =_sbss
    ldr r2, =_ebss
_zero_bss:
    cmp r1, r2
    bge _copy_data
    str r0, [r1]
    adds r1, r1, #4
    b _zero_bss

    # Copy .data
    ldr r1, =_sdata
    ldr r2, =_sdata_ram
    ldr r3, =_edata
_copy_data:
    cmp r1, r3
    bge _call_main
    str r2, [r1]
    adds r1, r1, #4
    adds r2, r2, #4
    b _copy_data

_call_main:
    cpsie i

    bl main
    b .

.thumb_func
.align 2
.global main
main:
    # Activate clock for PORTD
    ldr r1, =(SIM_ADDR + SIM_SCGC5)
    ldr r0, [r1] 
    ldr r2, =(1 << 12)
    orrs r0, r0, r2
    str r0, [r1]

    # Select GPIO to drive PTD5
    ldr r1, =(PORTD_ADDR)
    ldr r0, [r1, #5*PORT_PCR] 
    ldr r2, =(1 << 8)
    orrs r0, r0, r2
    str r0, [r1, #5*PORT_PCR]

    /* Project constants */
    ldr r6, =(GPIOD_ADDR)
    ldr r7, =(1 << 5)

    # Setup PTD5 to OUTPUT mode
    ldr r0, [r6, #GPIO_PDDR] 
    orrs r0, r0, r7
    str r0, [r6, #GPIO_PDDR]

_while:
    # Set PTD5
    str r7, [r6, #GPIO_PCOR]

    ldr r0, =1000
    bl _delay_ms

    # Clear PTD5
    str r7, [r6, #GPIO_PSOR]

    ldr r0, =1000
    bl _delay_ms

    b _while

.global _delay_ms
_delay_ms:
    ldr r1, =7000
    muls r1, r1, r0
_delay_ms_loop: 
    subs r1, r1, #1
    bne _delay_ms_loop
    mov pc, lr  
