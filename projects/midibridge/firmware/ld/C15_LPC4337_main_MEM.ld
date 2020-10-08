/* Define each memory region, for M4 core */
MEMORY
{
  MFlashA512 (   rx) : ORIGIN = 0x1a000000, LENGTH = 0x80000   /* 512K bytes (alias Flash)  -- code and initialized data images, for M4 and M0 */  
  MFlashB512    (rx) : ORIGIN = 0x1b000000, LENGTH = 0x80000   /* 512K bytes (alias Flash2) -- unused */  
  RamLoc40     (rwx) : ORIGIN = 0x10080000, LENGTH = 0xa000    /* 40K bytes  (alias RAM)    -- primary RAM (data & stack) for M4 */  
  RamAHB32     (rwx) : ORIGIN = 0x20000000, LENGTH = 0x8000    /* 32K bytes  (alias RAM2)   -- do not use (this is M0's private RAM), also RAM where 
                                                                  M0 code&ini-data is uploaded to. This must be the second RAM definition here! */  
  RamAHB16     (rwx) : ORIGIN = 0x20008000, LENGTH = 0x4000    /* 16K bytes  (alias RAM3)   -- shared RAM between M0 and M4*/  
  RamLoc32     (rwx) : ORIGIN = 0x10000000, LENGTH = 0x8000    /* 32K bytes  (alias RAM4)   -- unused */  
  RamAHB_ETB16 (rwx) : ORIGIN = 0x2000c000, LENGTH = 0x4000    /* 16K bytes  (alias RAM5)   -- unused */  
}

/* Define a symbol for base and top of each memory region */
  __base_MFlashA512 = 0x1a000000  ; /* MFlashA512 */  
  __base_Flash = 0x1a000000 ; /* Flash */  
  __top_MFlashA512 = 0x1a000000 + 0x80000 ; /* 512K bytes */  
  __top_Flash = 0x1a000000 + 0x80000 ; /* 512K bytes */  
  __base_MFlashB512 = 0x1b000000  ; /* MFlashB512 */  
  __base_Flash2 = 0x1b000000 ; /* Flash2 */  
  __top_MFlashB512 = 0x1b000000 + 0x80000 ; /* 512K bytes */  
  __top_Flash2 = 0x1b000000 + 0x80000 ; /* 512K bytes */  
  __base_RamLoc40 = 0x10080000  ; /* RamLoc40 */  
  __base_RAM = 0x10080000 ; /* RAM */  
  __top_RamLoc40 = 0x10080000 + 0xa000 ; /* 40K bytes */  
  __top_RAM = 0x10080000 + 0xa000 ; /* 40K bytes */  
  __base_RamAHB32 = 0x20000000  ; /* RamAHB32 */  
  __base_RAM2 = 0x20000000 ; /* RAM2 */  
  __top_RamAHB32 = 0x20000000 + 0x8000 ; /* 32K bytes */  
  __top_RAM2 = 0x20000000 + 0x8000 ; /* 32K bytes */  
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
