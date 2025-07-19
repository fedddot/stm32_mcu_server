ENTRY(Reset_Handler)

MEMORY {
    RAM (xrw)   : ORIGIN = 0x20000000,  LENGTH = 20K
    FLASH (rx)  : ORIGIN = 0x08000000,  LENGTH = 64K
}

g_stack_start = ORIGIN(RAM) + LENGTH(RAM);

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
        __rodata_start = .;
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH

    .ARM (READONLY) : {
        . = ALIGN(4);
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
        . = ALIGN(4);
    } >FLASH

    .data : {
        . = ALIGN(4);
        *(.data)
        *(.data*)
        PROVIDE ( s_data_end_ram = . );
        . = ALIGN(4);
    } >RAM AT> FLASH
    PROVIDE ( _data_start_ram = ADDR( .data ) );
    PROVIDE ( _data_size = SIZEOF( .data ) );
    PROVIDE ( _data_flash_addr = LOADADDR( .data ) );

    .tdata :  {
        . = ALIGN(4);
        g_tdata_start_ram = .;
        *(.tdata .tdata.* .gnu.linkonce.td.*)
        g_tdata_end_ram = .;
        . = ALIGN(4);
    } >RAM AT> FLASH
    g_tdata_flash_addr = LOADADDR(.tdata);

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

    ._user_heap_stack (NOLOAD) :
    {
        . = ALIGN(8);
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
    } >RAM

    /DISCARD/ :
    {
        libc.a:* ( * )
        libm.a:* ( * )
        libgcc.a:* ( * )
    }
}
