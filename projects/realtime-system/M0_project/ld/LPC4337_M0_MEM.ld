MEMORY
{
  /* Define each memory region */
  MFlashB512 (rx) : ORIGIN = 0x1b000000, LENGTH = 0x80000 /* 512K bytes */
  RamLoc32 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x8000 /* 32K bytes */
  RamLoc40 (rwx) : ORIGIN = 0x10080000, LENGTH = 0xa000 /* 40K bytes */


}
  /* Define a symbol for the top of each memory region */
  __top_MFlashB512 = 0x1b000000 + 0x80000;
  __top_RamLoc32 = 0x10000000 + 0x8000;
  __top_RamLoc40 = 0x10080000 + 0xa000;
