/*! linker script for memory layout */

/* Its parsed as C before used in linking! */

ENTRY(arch_startup)

SECTIONS {
    .boot_code MOVE_ADDR :
    {
        *?/boot/startup.asm.o ( * )
        *?/boot/copy.o ( * )
    }
    
    kernel_code_LMA = MOVE_ADDR + SIZEOF( .boot_code );
    
    .kernel_code kernel_code_LMA :
    {
        kernel_code_addr = .;
        *( .text* )
    }
    
    data_LMA = kernel_code_LMA + SIZEOF( .kernel_code );
    
    .data data_LMA :
    {
        *( .rodata* .data* )
    }
    
    bss_LMA = data_LMA + SIZEOF( .data );
    
    .bss bss_LMA :
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

