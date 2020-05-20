/* LPC4337 Memory Layout for LPC4337 C15 */

/* Define each memory region */
MEMORY
{
  /* Flash region */
  MFlashA512 (rx) : ORIGIN = 0x1a000000, LENGTH = 0x80000 /* 512K bytes  for M4 core */
  MFlashB512 (rx) : ORIGIN = 0x1b000000, LENGTH = 0x80000 /* 512K bytes  for M0 core */

  /* RAM region */
  RamLoc32 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x8000  /* 32k RAM (LOCAL)  */  /* will be used for M0 core exclusively */
  RamLoc40 (rwx) : ORIGIN = 0x10080000, LENGTH = 0xa000  /* 40k RAM (LOCAL)  */  /* will be used for M4 core exclusively */
  RamAHB32 (rwx) : ORIGIN = 0x20000000, LENGTH = 0x8000  /* 32k RAM (AHB)    */
  RamAHB16 (rwx) : ORIGIN = 0x20008000, LENGTH = 0x4000  /* 16k RAM (AHB)    */  /* for shared access between M0 and M4 cores */
}

/* Define a symbol for the top of each memory region */
  __top_MFlashA512 = 0x1a000000 + 0x80000;
  __top_MFlashB512 = 0x1b000000 + 0x80000;
  
  __top_RamLoc32   = 0x10000000 + 0x8000;
  __top_RamLoc40   = 0x10080000 + 0xa000;
  __top_RamAHB32   = 0x20000000 + 0x8000;
  __top_RamAHB16   = 0x20008000 + 0x4000;
