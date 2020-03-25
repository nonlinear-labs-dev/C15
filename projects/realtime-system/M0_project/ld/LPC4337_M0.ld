INCLUDE "LPC4337_M0_LIB.ld"
INCLUDE "LPC4337_M0_MEM.ld"

ENTRY(ResetISR)

SECTIONS
{

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
        LONG(LOADADDR(.data_RAM2));
        LONG(    ADDR(.data_RAM2));
        LONG(  SIZEOF(.data_RAM2));
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        LONG(    ADDR(.bss_RAM2));
        LONG(  SIZEOF(.bss_RAM2));
        __bss_section_table_end = .;
        __section_table_end = . ;
        /* End of Global Section Table */


        *(.after_vectors*)

    } >MFlashB512

    .text : ALIGN(4)
    {
         *(.text*)
        *(.rodata .rodata.* .constdata .constdata.*)
        . = ALIGN(4);

    } > MFlashB512

    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this.
     */
    .ARM.extab : ALIGN(4)
    {
    	*(.ARM.extab* .gnu.linkonce.armextab.*)
    } > MFlashB512
    __exidx_start = .;

    .ARM.exidx : ALIGN(4)
    {
    	*(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > MFlashB512
    __exidx_end = .;

    _etext = .;


    /* DATA section for RamLoc40 */
    .data_RAM2 : ALIGN(4)
    {
       FILL(0xff)
       PROVIDE(__start_data_RAM2 = .) ;
       *(.ramfunc.$RAM2)
       *(.ramfunc.$RamLoc40)
    	*(.data.$RAM2*)
    	*(.data.$RamLoc40*)
       . = ALIGN(4) ;
       PROVIDE(__end_data_RAM2 = .) ;
    } > RamLoc40 AT>MFlashB512

    /* MAIN DATA SECTION */


    .uninit_RESERVED : ALIGN(4)
    {
        KEEP(*(.bss.$RESERVED*))
        . = ALIGN(4) ;
        _end_uninit_RESERVED = .;
    } > RamLoc32


	/* Main DATA section (RamLoc32) */
	.data : ALIGN(4)
	{
	   FILL(0xff)
	   _data = . ;
	   *(vtable)
	   *(.ramfunc*)
	   *(.data*)
	   . = ALIGN(4) ;
	   _edata = . ;
	} > RamLoc32 AT>MFlashB512

    /* BSS section for RamLoc40 */
    .bss_RAM2 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM2 = .) ;
    	*(.bss.$RAM2*)
    	*(.bss.$RamLoc40*)
       . = ALIGN(4) ;
       PROVIDE(__end_bss_RAM2 = .) ;
    } > RamLoc40

    /* MAIN BSS SECTION */
    .bss : ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(end = .);
    } > RamLoc32

    /* NOINIT section for RamLoc40 */
    .noinit_RAM2 (NOLOAD) : ALIGN(4)
    {
    	*(.noinit.$RAM2*)
    	*(.noinit.$RamLoc40*)
       . = ALIGN(4) ;
    } > RamLoc40

    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD): ALIGN(4)
    {
        _noinit = .;
        *(.noinit*)
         . = ALIGN(4) ;
        _end_noinit = .;
    } > RamLoc32

    PROVIDE(_pvHeapStart = DEFINED(__user_heap_base) ? __user_heap_base : .);
    PROVIDE(_vStackTop = DEFINED(__user_stack_top) ? __user_stack_top : __top_RamLoc32 - 0);
}
