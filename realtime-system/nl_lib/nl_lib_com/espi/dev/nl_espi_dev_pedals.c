/******************************************************************************/
/**	@date		2016-02-29 DTZ
  	@author		2015-02-12 DTZ
*******************************************************************************/
#include <stdint.h>

#include "espi/dev/nl_espi_dev_pedals.h"

#include "espi/nl_espi_core.h"
#include "espi/nl_espi_adc.h"
#include "espi/nl_espi_io.h"

ESPI_ADC_T pedalsAdc;
ESPI_IO_T  setPullUps;
ESPI_IO_T  getPolarity;

static uint8_t pullResistorsNewDataToSend = 0;

void ESPI_DEV_Pedals_Init(void)
{
  ESPI_ADC_Init(&pedalsAdc, ESPI_ADC_3208, 4, 1);
  ESPI_IO_Init(&getPolarity, ESPI_SHIFT_IN, 1, 4, 2);

  ESPI_IO_Init(&setPullUps, ESPI_SHIFT_OUT, 2, 4, 3);
}

void ESPI_DEV_Pedals_EspiPullChannel_Blocking(uint8_t channel)
{
  ESPI_ADC_Channel_Poll_Blocking(&pedalsAdc, channel);
}

/******************************************************************************/
/** @param[]  id: 1 .. 4
    @return   12 adc value, right aligned
*******************************************************************************/
uint16_t ESPI_DEV_Pedals_GetValue(uint8_t id)
{
  return ESPI_ADC_Channel_Get(&pedalsAdc, id);
}

/******************************************************************************/
/** Pedal states
	The six possible states for each pedal are exclusive!

	@param[in]	pedal: 1..4
	@param[in]  state: - PEDALS_TEST_POLARITY
					   - PEDALS_TIP_TO_5V
					   - PEDALS_TIP_TO_PULLUP
					   - PEDALS_RING_TO_5V
					   - PEDALS_RING_TO_PULLUP
					   - PEDALS_DEFAULT_OFF
	@info
		   val[0]     ||        val[1]           | Byte

	   4    |    3    ||    2    |    1     | Pedal
	7 6 5 4 | 3 2 1 0 || 7 6 5 4 | 3 2 1 0  | Bit
	A B C D | E F G H || A B C D | E F G H  | Output Pin (Shift Reg)

	   4    |    3    ||    2    |    1     | Pedal
	R R T T | R R T T || R R T T | R R T T  | R:Ring T:Tip
	5 P 5 P | 5 P 5 P || 5 P 5 P | 5 P 5 P  | 5:5V   P:PullUp
*******************************************************************************/
void ESPI_DEV_Pedals_SetPedalState(uint8_t pedal, uint8_t state)
{
  if ((pedal < 1) || (pedal > 4))
  {
    pedal = 1;
  }

  switch (state)
  {
    case PEDAL_CHECK_PINCONFIG:
    case PEDAL_TIP_TO_5V:
    case PEDAL_TIP_TO_PULLUP:
    case PEDAL_RING_TO_5V:
    case PEDAL_RING_TO_PULLUP:
    case PEDAL_DEFAULT_OFF:
    {
      switch (pedal)
      {
        case 1:
          setPullUps.val[1] = (setPullUps.val[1] & 0b11110000) | state;
          break;
        case 2:
          setPullUps.val[1] = (setPullUps.val[1] & 0b00001111) | (state << 4);
          break;
        case 3:
          setPullUps.val[0] = (setPullUps.val[0] & 0b11110000) | state;
          break;
        case 4:
          setPullUps.val[0] = (setPullUps.val[0] & 0b00001111) | (state << 4);
          break;
        default:
          break;
      }
      pullResistorsNewDataToSend = 1;
      break;
    }
    default:
      break;
  }
}

void ESPI_DEV_Pedals_PullResistors_EspiSend(void)
{
  ESPI_IO_Out_Process(&setPullUps);
}

void ESPI_DEV_Pedals_PullResistors_EspiSendIfChanged(void)
{
  if (pullResistorsNewDataToSend == 1)
  {
    ESPI_IO_Out_Process(&setPullUps);
    pullResistorsNewDataToSend = 0;
  }
}

/******************************************************************************/
/** Detect
*******************************************************************************/
uint8_t ESPI_DEV_Pedals_Detect_GetValue(void)
{
  return getPolarity.val[0];
}

void ESPI_DEV_Pedals_Detect_EspiPull(void)
{
  ESPI_IO_In_Process(&getPolarity);
}
