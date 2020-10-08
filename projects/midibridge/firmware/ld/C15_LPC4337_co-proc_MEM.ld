/* Define each memory region, for M0 core -- running from RAM */
MEMORY
{
  RamAHB32     (rwx) : ORIGIN = 0x20000000, LENGTH = 0x8000 /* 32K bytes (alias RAM)  -- primary RAM (data & stack), 
                                                               and code and inititalized data image, will be uploaded by M4 */  
  RamLoc40     (rwx) : ORIGIN = 0x10080000, LENGTH = 0xa000 /* 40K bytes (alias RAM2) -- do not use (this is M4's private RAM) */  
  RamAHB16     (rwx) : ORIGIN = 0x20008000, LENGTH = 0x4000 /* 16K bytes (alias RAM3) -- shared RAM between M0 and M4*/  
  RamLoc32     (rwx) : ORIGIN = 0x10000000, LENGTH = 0x8000 /* 32K bytes (alias RAM4) -- unused */  
  RamAHB_ETB16 (rwx) : ORIGIN = 0x2000c000, LENGTH = 0x4000 /* 16K bytes (alias RAM5) -- unused */  
}

/* Define a symbol for base and top of each memory region */
  __base_RamAHB32 = 0x20000000  ; /* RamAHB32 */  
  __base_RAM = 0x20000000 ; /* RAM */  
  __top_RamAHB32 = 0x20000000 + 0x8000 ; /* 32K bytes */  
  __top_RAM = 0x20000000 + 0x8000 ; /* 32K bytes */  
  __base_RamLoc40 = 0x10080000  ; /* RamLoc40 */  
  __base_RAM2 = 0x10080000 ; /* RAM2 */  
  __top_RamLoc40 = 0x10080000 + 0xa000 ; /* 40K bytes */  
  __top_RAM2 = 0x10080000 + 0xa000 ; /* 40K bytes */  
  __base_RamAHB16 = 0x20008000  ; /* RamAHB16 */  
  __base_RAM3 = 0x20008000 ; /* RAM3 */  
  __top_RamAHB16 = 0x20008000 + 0x4000 ; /* 16K bytes */  
  __top_RAM3 = 0x20008000 + 0x4000 ; /* 16K bytes */  
  __base_RamLoc32 = 0x10000000  ; /* RamLoc32 */  
  __base_RAM4 = 0x10000000 ; /* RAM4 */  
  __top_RamLoc32 = 0x10000000 + 0x8000 ; /* 32K bytes */  
  __top_RAM4 = 0x10000000 + 0x8000 ; /* 32K bytes */  
  __base_RamAHB_ETB16 = 0x2000c000  ; /* RamAHB_ETB16 */  
  __base_RAM5 = 0x2000c000 ; /* RAM5 */  
  __top_RamAHB_ETB16 = 0x2000c000 + 0x4000 ; /* 16K bytes */  
  __top_RAM5 = 0x2000c000 + 0x4000 ; /* 16K bytes */  
