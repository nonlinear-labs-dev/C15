/******************************************************************************/
/** @file		emphase_ipc.c
    @date		2020-05-24 KSTR
    @brief    	M4 <-> M0 inter processor communication module
    @author		Nemanja Nikodijevic 2015-02-03
*******************************************************************************/
#include <stdint.h>
#include "ipc/emphase_ipc.h"

//
// Places the variable(s) in the shared memory block.
// This block must be defined in C15_LPC4337_MEM.ld.
// The section used must be defined in both C15_LPC4337_M0.ld and C15_LPC4337_M4.ld identically
// and must be defined properly and identically in the *_MEM.ld script includes.
// When the size of all shared variables is larger than the block the linker will throw an error.
// To make sure all variable are at the same memory location and have the same layout,
// use only one single struct and embed all your variables there.
// The section is defined as "noinit" so you must clear the data yourself.
//
__attribute__((section(".data.$RamAHB16"))) SharedData_T s;

// double-check the used memory
void CheckSizeAtCompileTime(void)
{
  (void) sizeof(char[-!(sizeof(s) < 4000)]);  // must be less than the 4096 bytes in shared mem
}

/******************************************************************************/
/**	@brief  setup and clear data in shared memory
*******************************************************************************/
void Emphase_IPC_Init(void)
{
  s.keyBufferWritePos = 0;
  s.keyBufferReadPos  = 0;

  for (int i = 0; i < EMPHASE_IPC_KEYBUFFER_SIZE; i++)
    s.keyBufferData[i] = 0;

  for (int i = 0; i < IPC_ADC_NUMBER_OF_CHANNELS; i++)
  {
    for (int k = 0; k < IPC_ADC_BUFFER_SIZE; k++)
      s.adcBufferData.values[i][k] = IPC_ADC_DEFAULT;
    s.adcBufferData.sum[i] = IPC_ADC_DEFAULT * IPC_ADC_BUFFER_SIZE;
  }
  s.adcConfigData       = 0;
  s.adcBufferReadIndex  = 0;
  s.adcBufferWriteIndex = 0;
  s.ticker              = 0;
  s.ADCTicks            = 0;
  s.RitCrtlReg          = 0;
}
