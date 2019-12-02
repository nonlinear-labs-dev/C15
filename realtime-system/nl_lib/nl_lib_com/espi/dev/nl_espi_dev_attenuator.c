/******************************************************************************/
/** @file		nl_espi_attenuator.c
    @date		2015-02-19 DTZ
    @brief    	eSPI module for Attenuator
    @author		Nemanja Nikodijevic 2014-07-07
*******************************************************************************/
#include <espi/dev/nl_espi_dev_attenuator.h>

static uint8_t att_channel[2];
static uint8_t is_changed[2];
static uint8_t rxb[2], txb[2];

static void ESPI_Attenuator_Callback(uint32_t status)
{
  ESPI_SCS_Select(ESPI_PORT_OFF, ESPI_ATTENUATOR_DEVICE);
  //scu_pinmux(3,7, MD_PLN_FAST, 4);
}

void ESPI_DEV_Attenuator_Init(void)
{
  att_channel[0] = att_channel[1] = 0;
  is_changed[0] = is_changed[1] = 0;
}

void ESPI_Attenuator_Channel_Set(uint8_t ch, uint8_t val)
{
  if (ch >= 2)
    return;
  if (att_channel[ch] == val)
    return;
  att_channel[ch] = val;
  is_changed[ch]  = 1;
}

uint32_t ESPI_Attenuator_EspiSendIfChanged(uint8_t ch)
{
  if (ch >= 2)
    return 0;

  if (is_changed[ch] == 0)
    return 2;
  is_changed[ch] = 0;

  ESPI_SCS_Select(ESPI_ATTENUATOR_PORT, ESPI_ATTENUATOR_DEVICE);

  txb[0] = ch;
  txb[1] = att_channel[ch];

  return ESPI_Transfer(txb, rxb, 2, ESPI_Attenuator_Callback);
}

uint32_t ESPI_Attenuator_EspiSend(uint8_t ch)
{
  if (ch >= 2)
    return 0;

  ESPI_SCS_Select(ESPI_ATTENUATOR_PORT, ESPI_ATTENUATOR_DEVICE);

  txb[0] = ch;
  txb[1] = att_channel[ch];

  return ESPI_Transfer(txb, rxb, 2, ESPI_Attenuator_Callback);
}
