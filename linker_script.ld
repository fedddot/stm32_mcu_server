ENTRY(Reset_Handler)

MEMORY {
    RAM (xrw)   : ORIGIN = 0x20000000,  LENGTH = 20K
    FLASH (rx)  : ORIGIN = 0x08000000,  LENGTH = 64K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

_Min_Heap_Size = 0x200;
_Min_Stack_Size = 0x400;

SECTIONS {
    .isr_vector : {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } >FLASH

    .text : {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        . = ALIGN(4);
    } >FLASH

    .rodata : {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH

    .data : {
        . = ALIGN(4);
        __data_start = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
    } >RAM AT> FLASH

    .tdata :  {
        . = ALIGN(4);
        *(.tdata .tdata.* .gnu.linkonce.td.*)
        . = ALIGN(4);
    } >RAM AT> FLASH

    .tbss (NOLOAD) : {
        . = ALIGN(4);
        *(.tbss .tbss.*)
        . = ALIGN(4);
    } >RAM

    .bss (NOLOAD) : {
        . = ALIGN(4);
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
    } >RAM

    /DISCARD/ :
    {
        libc.a:* ( * )
        libm.a:* ( * )
        libgcc.a:* ( * )
    }
}
