MEMORY
{
  /* Define each memory region */
  MFlashA512 (rx) : ORIGIN = 0x1a000000, LENGTH = 0x80000 /* 512K bytes */
  RamLocM4 (rwx) : ORIGIN = 0x10080000, LENGTH = 0xa000 /* 40K bytes */
  RamShared (rwx) : ORIGIN = 0x10007000, LENGTH = 0x1000 /* 4K bytes */


}
  /* Define a symbol for the top of each memory region */
  __top_MFlashA512 = 0x1a000000 + 0x80000;
  __top_RamLocM4 = 0x10080000 + 0xa000;
  __top_RamShared = 0x10007000 + 0x1000;
