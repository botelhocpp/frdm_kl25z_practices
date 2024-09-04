.syntax unified

.section .vectors, "a"
    .align 2
    .long  _estack
    .long  _start

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
    bl main
    b .
