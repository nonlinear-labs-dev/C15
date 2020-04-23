/* LPC4337 Memory Layout for LPC4337 C15 */

MEMORY
{
  /* Define each memory region */
  MFlashA512 (rx) : ORIGIN = 0x1a000000, LENGTH = 0x80000 /* 512K bytes  for M4 core */
  MFlashB512 (rx) : ORIGIN = 0x1b000000, LENGTH = 0x80000 /* 512K bytes  for M0 core */
  RamLocM4  (rwx) : ORIGIN = 0x10080000, LENGTH = 0xa000  /* 40K bytes -- all of the 40k RAM block for M4 core */
  RamLocM0  (rwx) : ORIGIN = 0x10000000, LENGTH = 0x7000  /* 28K bytes -- first 28k of the 32k RAM block for M0 core */
  RamShared (rwx) : ORIGIN = 0x10007000, LENGTH = 0x1000  /* 4K bytes  -- remainder of the 32k RAM block shared for both cores */


}
  /* Define a symbol for the top of each memory region */
  __top_MFlashA512 = 0x1a000000 + 0x80000;
  __top_MFlashB512 = 0x1b000000 + 0x80000;
  __top_RamLocM4   = 0x10080000 + 0xa000;
  __top_RamLocM0   = 0x10000000 + 0x7000;
  __top_RamShared  = 0x10007000 + 0x1000;
