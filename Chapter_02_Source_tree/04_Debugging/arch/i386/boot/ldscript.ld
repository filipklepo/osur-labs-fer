/*! linker script for memory layout */

/* Its parsed as C before used in linking! */

ENTRY(arch_startup)

SECTIONS {
    .boot_code LOAD_ADDR :
    {
        *?/boot/startup.asm.o ( * )
        *?/boot/copy.o ( * )
    }
    
    kernel_code_LMA = LOAD_ADDR + SIZEOF( .boot_code );
    kernel_code_VMA = MOVE_ADDR;
    
    .kernel_code kernel_code_VMA : AT (kernel_code_LMA)
    {
        kernel_code_addr = .;
        *( .text* )
    }
    
    kernel_code_size = SIZEOF( .kernel_code );
    data_LMA = kernel_code_LMA + kernel_code_size;
    data_VMA = kernel_code_VMA + kernel_code_size;
    
    .data data_VMA : AT (data_LMA)
    {
        *( .rodata* .data* )
    }
    
    data_size = SIZEOF ( .data );
    bss_LMA = data_LMA + data_size;
    bss_VMA = data_VMA + data_size;
    
    .bss bss_VMA : AT (bss_LMA)
    {
        *( .bss* COMMON* )
        . = ALIGN (4096);
    }

    kernel_end_LMA = .;

    /* Program, I think, should work without line below */
    stack_start_addr = STACK_ADDR - STACK_SIZE;
    stack_top_addr = STACK_ADDR;

    /DISCARD/ : { *(.eh_frame*) } 

#ifndef DEBUG
    /DISCARD/ : { *(*) } 
#endif
    kernel_end_addr = .;
}

