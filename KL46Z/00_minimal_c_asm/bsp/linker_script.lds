ENTRY(_start);

MEMORY {
    interrupt(rx)  : ORIGIN = 0x00000000, LENGTH = 0xC0
    cfmprotrom(rx) : ORIGIN = 0x00000400, LENGTH = 0x10
    flash(rx)      : ORIGIN = 0x00000410, LENGTH = 128K - 0x410
    sram(rwx)      : ORIGIN = 0x1ffff000, LENGTH = 16K
}

_estack = ORIGIN(sram) + LENGTH(sram);

SECTIONS {
    .vectors : { 
        . = ALIGN(4);
        KEEP(*(.vectors));
        . = ALIGN(4);
    } > interrupt

    /* Flash Configuration Field (FCF) */
    .cfmprotect :
    {
        . = ALIGN(4);
        KEEP(*(.cfmconfig))
        . = ALIGN(4);
    } > cfmprotrom

    .text :
    {
        *(.text*)
    } > flash

    .rodata : {
        *(.rodata*)
    } > flash

    .data : {
        _sdata = .;
        *(.first_data)
        *(.data SORT(.data.*))
        _edata = .;
    } > sram AT > flash
    _sdata_ram = LOADADDR(.data);
    
    .bss : {
        _sbss = .;
        *(.bss SORT(.bss.*) COMMON)
        _ebss = .;
    } > sram

    _end = .;
}
