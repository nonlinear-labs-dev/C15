INCLUDE "C15_LPC4337_main_MEM.ld"

ENTRY(ResetISR)

SECTIONS
{
  
    .text_Flash2 : ALIGN(4)
    {
       FILL(0xff)
    	*(.text_Flash2*) /* for compatibility with previous releases */
    	*(.text_MFlashB512*) /* for compatibility with previous releases */
    	*(.text.$Flash2*)
    	*(.text.$MFlashB512*)
    	*(.rodata.$Flash2*)
    	*(.rodata.$MFlashB512*)
    } > MFlashB512

    /* MAIN TEXT SECTION */
    .text : ALIGN(4)
    {
        FILL(0xff)
        __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))
        /* Global Section Table */
        . = ALIGN(4) ; 
        __section_table_start = .;
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        LONG(LOADADDR(.data_RAM2_core_m0app_text));
        LONG(    ADDR(.data_RAM2_core_m0app_text));
        LONG(  SIZEOF(.data_RAM2_core_m0app_text));
        LONG(LOADADDR(.data_RAM2_core_m0app_ARM_extab));
        LONG(    ADDR(.data_RAM2_core_m0app_ARM_extab));
        LONG(  SIZEOF(.data_RAM2_core_m0app_ARM_extab));
        LONG(LOADADDR(.data_RAM2_core_m0app_ARM_exidx));
        LONG(    ADDR(.data_RAM2_core_m0app_ARM_exidx));
        LONG(  SIZEOF(.data_RAM2_core_m0app_ARM_exidx));
        LONG(LOADADDR(.data_RAM2_core_m0app_data));
        LONG(    ADDR(.data_RAM2_core_m0app_data));
        LONG(  SIZEOF(.data_RAM2_core_m0app_data));
        LONG(LOADADDR(.data_RAM2));
        LONG(    ADDR(.data_RAM2));
        LONG(  SIZEOF(.data_RAM2));
        LONG(LOADADDR(.data_RAM3));
        LONG(    ADDR(.data_RAM3));
        LONG(  SIZEOF(.data_RAM3));
        LONG(LOADADDR(.data_RAM4));
        LONG(    ADDR(.data_RAM4));
        LONG(  SIZEOF(.data_RAM4));
        LONG(LOADADDR(.data_RAM5));
        LONG(    ADDR(.data_RAM5));
        LONG(  SIZEOF(.data_RAM5));
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        LONG(    ADDR(.bss_RAM2));
        LONG(  SIZEOF(.bss_RAM2));
        LONG(    ADDR(.bss_RAM3));
        LONG(  SIZEOF(.bss_RAM3));
        LONG(    ADDR(.bss_RAM4));
        LONG(  SIZEOF(.bss_RAM4));
        LONG(    ADDR(.bss_RAM5));
        LONG(  SIZEOF(.bss_RAM5));
        __bss_section_table_end = .;
        __section_table_end = . ;
	    /* End of Global Section Table */

        *(.after_vectors*)

    } >MFlashA512

    .text : ALIGN(4)    
    {
        *(.text*)
        *(.rodata .rodata.* .constdata .constdata.*)
        . = ALIGN(4);
    } > MFlashA512
    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this. 
     */
    .ARM.extab : ALIGN(4) 
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > MFlashA512
    __exidx_start = .;

    .ARM.exidx : ALIGN(4)
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > MFlashA512
    __exidx_end = .;

    _etext = .;
        
    /* DATA section for RamAHB32 */
  
    .data_RAM2_core_m0app_text : SUBALIGN(4)
    {
       FILL(0xff)
       PROVIDE(__start_data_RAM2 = .) ;
       __core_m0app_START__ = .; /* start of slave image */
       KEEP(*(.core_m0app))
    } > RamAHB32 AT>MFlashA512

    /* M0APP extab and exidx sections */
    .data_RAM2_core_m0app_ARM_extab . : SUBALIGN(4)
    {
        FILL(0xff)
        KEEP(*(.core_m0app.ARM.extab))
    } > RamAHB32 AT>MFlashA512 

    .data_RAM2_core_m0app_ARM_exidx . : SUBALIGN(4)
    {
        FILL(0xff)
        KEEP(*(.core_m0app.ARM.exidx))
    } > RamAHB32 AT>MFlashA512 

    /* M0APP data section */
    .data_RAM2_core_m0app_data . : SUBALIGN(4)
    {
        FILL(0xff)
        KEEP(*(.core_m0app.data_*)) KEEP(*(.core_m0app.data))
        __core_m0app_END__ = .; /* end of slave image */

        /* perform some simple sanity checks */
        ASSERT(!(__core_m0app_START__ == __core_m0app_END__), "No slave code for _core_m0app");
        ASSERT( (ABSOLUTE(__core_m0app_START__) == __vectors_start___core_m0app), "M0APP execute address differs from address provided in source image");
    } > RamAHB32 AT>MFlashA512 
    .data_RAM2 : ALIGN(4)
    {
        FILL(0xff)
        *(.ramfunc.$RAM2)
        *(.ramfunc.$RamAHB32)
        *(.data.$RAM2*)
        *(.data.$RamAHB32*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM2 = .) ;
     } > RamAHB32 AT>MFlashA512

    /* DATA section for RamAHB16 */
  
    .data_RAM3 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM3 = .) ;
        *(.ramfunc.$RAM3)
        *(.ramfunc.$RamAHB16)
        *(.data.$RAM3*)
        *(.data.$RamAHB16*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM3 = .) ;
     } > RamAHB16 AT>MFlashA512

    /* DATA section for RamLoc32 */
  
    .data_RAM4 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM4 = .) ;
        *(.ramfunc.$RAM4)
        *(.ramfunc.$RamLoc32)
        *(.data.$RAM4*)
        *(.data.$RamLoc32*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM4 = .) ;
     } > RamLoc32 AT>MFlashA512

    /* DATA section for RamAHB_ETB16 */
  
    .data_RAM5 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM5 = .) ;
        *(.ramfunc.$RAM5)
        *(.ramfunc.$RamAHB_ETB16)
        *(.data.$RAM5*)
        *(.data.$RamAHB_ETB16*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM5 = .) ;
     } > RamAHB_ETB16 AT>MFlashA512

    /* MAIN DATA SECTION */
    .uninit_RESERVED : ALIGN(4)
    {
        KEEP(*(.bss.$RESERVED*))
        . = ALIGN(4) ;
        _end_uninit_RESERVED = .;
    } > RamLoc40
    /* Main DATA section (RamLoc40) */
    .data : ALIGN(4)
    {
       FILL(0xff)
       _data = . ;
       *(vtable)
       *(.ramfunc*)
       *(.data*)
       . = ALIGN(4) ;
       _edata = . ;
    } > RamLoc40 AT>MFlashA512
    /* BSS section for RamAHB32 */
    .bss_RAM2 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM2 = .) ;
       *(.bss.$RAM2*)
       *(.bss.$RamAHB32*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM2 = .) ;
    } > RamAHB32 
    /* BSS section for RamAHB16 */
    .bss_RAM3 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM3 = .) ;
       *(.bss.$RAM3*)
       *(.bss.$RamAHB16*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM3 = .) ;
    } > RamAHB16 
    /* BSS section for RamLoc32 */
    .bss_RAM4 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM4 = .) ;
       *(.bss.$RAM4*)
       *(.bss.$RamLoc32*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM4 = .) ;
    } > RamLoc32 
    /* BSS section for RamAHB_ETB16 */
    .bss_RAM5 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM5 = .) ;
       *(.bss.$RAM5*)
       *(.bss.$RamAHB_ETB16*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM5 = .) ;
    } > RamAHB_ETB16 
    /* MAIN BSS SECTION */
    .bss : ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(end = .);
    } > RamLoc40
    /* NOINIT section for RamAHB32 */
    .noinit_RAM2 (NOLOAD) : ALIGN(4)
    {
       *(.noinit.$RAM2*)
       *(.noinit.$RamAHB32*)
       . = ALIGN(4) ;
    } > RamAHB32 
    /* NOINIT section for RamAHB16 */
    .noinit_RAM3 (NOLOAD) : ALIGN(4)
    {
       *(.noinit.$RAM3*)
       *(.noinit.$RamAHB16*)
       . = ALIGN(4) ;
    } > RamAHB16 
    /* NOINIT section for RamLoc32 */
    .noinit_RAM4 (NOLOAD) : ALIGN(4)
    {
       *(.noinit.$RAM4*)
       *(.noinit.$RamLoc32*)
       . = ALIGN(4) ;
    } > RamLoc32 
    /* NOINIT section for RamAHB_ETB16 */
    .noinit_RAM5 (NOLOAD) : ALIGN(4)
    {
       *(.noinit.$RAM5*)
       *(.noinit.$RamAHB_ETB16*)
       . = ALIGN(4) ;
    } > RamAHB_ETB16 
    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD): ALIGN(4)
    {
        _noinit = .;
        *(.noinit*) 
         . = ALIGN(4) ;
        _end_noinit = .;
    } > RamLoc40

/*    PROVIDE(_pvHeapStart = DEFINED(__user_heap_base) ? __user_heap_base : .); */
/*    PROVIDE(_vStackTop = DEFINED(__user_stack_top) ? __user_stack_top : __top_RamLoc40 - 0); */
    PROVIDE(_vStackTop = __top_RamLoc40 - 0);

    /* ## Create checksum value (used in startup) ## */
    PROVIDE(__valid_user_code_checksum = 0 - 
                                         (_vStackTop 
                                         + (ResetISR + 1) 
                                         + (NMI_Handler + 1) 
                                         + (HardFault_Handler + 1) 
                                         + (( DEFINED(MemManage_Handler) ? MemManage_Handler : 0 ) + 1)   /* MemManage_Handler may not be defined */
                                         + (( DEFINED(BusFault_Handler) ? BusFault_Handler : 0 ) + 1)     /* BusFault_Handler may not be defined */
                                         + (( DEFINED(UsageFault_Handler) ? UsageFault_Handler : 0 ) + 1) /* UsageFault_Handler may not be defined */
                                         ) );
}