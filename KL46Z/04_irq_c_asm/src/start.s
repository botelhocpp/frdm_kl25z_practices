.syntax unified

.equ SIM_COPC, 0x40048100

# Vector table
.section .vectors, "a"
    .align 2
    .long  _estack
    .long  _reset_handler
    .skip 4*13
    .long _systick_handler
    .skip 4*31
    .long _portc_handler
    
# Vector table
.section .cfmconfig, "a"
    .long 0xFFFFFFFF
    .long 0xFFFFFFFF
    .long 0xFFFFFFFF
    .long 0xFFFFFFFE

.section .text
.thumb_func
.align 2
.global _reset_handler
_reset_handler:
    cpsid i

    eors r0, r0, r0

    # Disable Watchdogs
    ldr r1, =SIM_COPC
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
    ldr r3, [r2]
    str r3, [r1]
    adds r1, r1, #4
    adds r2, r2, #4
    b _copy_data

_call_main:
    cpsie i

    bl main
    b .

.align	1
.thumb_func
.weak _dummy_isr
_dummy_isr:
    b .

.macro def_dummy_isr    isr_name
    .weak \isr_name
    .set  \isr_name, _dummy_isr
.endm

def_dummy_isr _systick_handler
def_dummy_isr _portc_handler
