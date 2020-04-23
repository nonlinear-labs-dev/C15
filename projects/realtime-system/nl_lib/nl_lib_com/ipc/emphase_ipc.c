/******************************************************************************/
/** @file		emphase_ipc.h
    @date		2016-02-29 DTZ
    @brief    	M4 <-> M0 inter processor communication module
    @author		Nemanja Nikodijevic 2015-02-03
*******************************************************************************/
#include <stdint.h>
#include "ipc/emphase_ipc.h"
#include "drv/nl_rit.h"

//
// Places the variable(s) in the shared memory block.
// This block must be defined in both LPC4337_M0_MEM.ld and LPC4337_M4_MEM.ld identically.
// The section ".shared" must be defined in both LPC4337_M0_MEM.ld and LPC4337_M4_MEM.ld identically.
// When the size of all shared variables is larger than the block the linker will throw an error.
// To make sure all variable are at the same memory location and have the same layout,
// use only one struct and embed your variables there.
// The section is defined as "noinit" so you must clear the data yourself.
//
__attribute__((section(".shared"))) SharedData_T s;

// double check the used memory
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
  {
    s.keyBufferData[i].key       = 0;
    s.keyBufferData[i].timeInUs  = 0;
    s.keyBufferData[i].direction = 0;
  }

  for (int i = 0; i < IPC_ADC_NUMBER_OF_CHANNELS; i++)
  {
    for (int k = 0; k < IPC_ADC_BUFFER_SIZE; k++)
      s.adcBufferData.values[i][k] = IPC_ADC_DEFAULT;
    s.adcBufferData.sum[i] = IPC_ADC_DEFAULT * IPC_ADC_BUFFER_SIZE;
  }
  s.adcConfigData       = 0;
  s.adcBufferReadIndex  = 0;
  s.adcBufferWriteIndex = 0;
  s.timer               = 0;
  s.KBSTicks            = 0;
  s.SchedulerTicks      = 0;
}
