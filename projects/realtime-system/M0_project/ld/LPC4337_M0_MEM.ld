MEMORY
{
  /* Define each memory region */
  MFlashB512 (rx) : ORIGIN = 0x1b000000, LENGTH = 0x80000 /* 512K bytes */
  RamLocM0 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x7000 /* 28K bytes */
  RamShared (rwx) : ORIGIN = 0x10007000, LENGTH = 0x1000 /* 4K bytes */


}
  /* Define a symbol for the top of each memory region */
  __top_MFlashB512 = 0x1b000000 + 0x80000;
  __top_RamLocM0 = 0x10000000 + 0x7000;
  __top_RamShared = 0x10007000 + 0x1000;
