/******************************************************************************/
/** @file		nl_gpdma.c
    @date		2014-10-09
    @brief    	Basic GPDMA functions
    @author		Nemanja Nikodijevic [2014-10-09]
*******************************************************************************/
#include "drv/nl_gpdma.h"
#include "cmsis/lpc43xx_gpdma.h"

static TransferCallback NL_GPDMA_Callbacks[8];
static uint8_t          channels    = 0;
static uint8_t          initialized = 0;

/**
 * @brief Lookup Table of GPDMA Channel Number matched with
 * GPDMA channel pointer
 */
static LPC_GPDMACH_TypeDef *pGPDMAC[8] = {
  LPC_GPDMACH0,  // GPDMA Channel 0
  LPC_GPDMACH1,  // GPDMA Channel 1
  LPC_GPDMACH2,  // GPDMA Channel 2
  LPC_GPDMACH3,  // GPDMA Channel 3
  LPC_GPDMACH4,  // GPDMA Channel 4
  LPC_GPDMACH5,  // GPDMA Channel 5
  LPC_GPDMACH6,  // GPDMA Channel 6
  LPC_GPDMACH7,  // GPDMA Channel 7
};

void NL_GPDMA_Init(uint8_t ch)
{
  uint8_t i;

  if (initialized)
    return;
  initialized = 1;

  channels = ch;
  for (i = 0; i < 8; i++)
  {
    if (ch & (1 << i))
    {
      pGPDMAC[i]->CConfig   = 0;
      LPC_GPDMA->INTTCCLEAR = 1 << i;
      LPC_GPDMA->INTERRCLR  = 1 << i;
      NL_GPDMA_Callbacks[i] = NULL;
    }
  }

#if 0
	/* Initialize GPDMA */
	/* Reset all channel configuration register */
	pGPDMAC[0]->CConfig = 0;
	pGPDMAC[1]->CConfig = 0;
	pGPDMAC[2]->CConfig = 0;
	pGPDMAC[3]->CConfig = 0;
	pGPDMAC[4]->CConfig = 0;
	pGPDMAC[5]->CConfig = 0;
	pGPDMAC[6]->CConfig = 0;
	pGPDMAC[7]->CConfig = 0;

	/* Clear all DMA interrupt and error flag */
	LPC_GPDMA->INTTCCLEAR = 0xFF;
	LPC_GPDMA->INTERRCLR = 0xFF;

	NVIC_DisableIRQ(DMA_IRQn);
#endif
  /** Setup DMAMUX register in CREG for the desired configuration,
	 *  DMA peripheral connection should be configured from each driver
	 */
  LPC_CREG->DMAMUX = 0x15694;
}

/******************************************************************************/
/** @brief    	Function for GPDMA polling, it polls channels defined in
 * 				NL_GPDMA_Init() for the desired setup. It serves and cleans up
 * 				the GPDMA channel interrupts. For each channel it calls the
 * 				corresponding callback, with SUCCESS or ERROR, depending on
 * 				the cause of the interrupt. In order to reuse the channel for
 * 				the transfer, this function needs to cleanup that channel first.
 * 				This means that this function should be called after
 * 				the desired transfer has finished.
*******************************************************************************/
void NL_GPDMA_Poll(void)
{
  uint32_t             tmp;
  LPC_GPDMACH_TypeDef *pDMAch;
  /* Scan interrupt pending */
  for (tmp = 0; tmp < 8; tmp++)
  {
    if ((channels & (1 << tmp)) == 0)
      continue;

    if (LPC_GPDMA->INTSTAT & GPDMA_DMACIntStat_Ch(tmp))
    {

      /* Check counter terminal status */
      if (LPC_GPDMA->INTTCSTAT & GPDMA_DMACIntTCStat_Ch(tmp))
      {
        // Clear terminate counter Interrupt pending
        LPC_GPDMA->INTTCCLEAR = GPDMA_DMACIntTCClear_Ch(tmp);
        /* Disable the channel */
        pDMAch = (LPC_GPDMACH_TypeDef *) pGPDMAC[tmp];
#if 0
			if(pDMAch->CLLI)
				continue;
#endif
        pDMAch->CConfig &= ~GPDMA_DMACCxConfig_E;

        if (NL_GPDMA_Callbacks[tmp])
          NL_GPDMA_Callbacks[tmp](SUCCESS);
      }
      /* Check error terminal status */
      if (LPC_GPDMA->INTERRSTAT & GPDMA_DMACIntErrStat_Ch(tmp))
      {
        // Clear error counter Interrupt pending
        LPC_GPDMA->INTERRCLR = GPDMA_DMACIntErrClr_Ch(tmp);
        /* Disable the channel */
        pDMAch = (LPC_GPDMACH_TypeDef *) pGPDMAC[tmp];
        pDMAch->CConfig &= ~GPDMA_DMACCxConfig_E;

        if (NL_GPDMA_Callbacks[tmp])
          NL_GPDMA_Callbacks[tmp](ERROR);
      }
    }
  }
}

Status NL_GPDMA_SetupChannel(NL_GPDMA_ChDesc *desc, TransferCallback callback)
{
  LPC_GPDMACH_TypeDef *pDMAch;
  uint8_t              SrcPeripheral = 0, DestPeripheral = 0;
  uint32_t             TransferType = 0;
  uint8_t              chan;

  chan = desc->channel;

  if ((chan >= 8) || ((channels & (1 << chan)) == 0))
    return ERROR;

  if (LPC_GPDMA->ENBLDCHNS & (GPDMA_DMACEnbldChns_Ch(chan)))
  {
    /* This channel is already enabled, return ERROR */
    return ERROR;
  }

  pDMAch = (LPC_GPDMACH_TypeDef *) pGPDMAC[chan];

  /* Reset the Interrupt status */
  LPC_GPDMA->INTTCCLEAR = GPDMA_DMACIntTCClear_Ch(chan);
  LPC_GPDMA->INTERRCLR  = GPDMA_DMACIntErrClr_Ch(chan);

  /* Clear DMA configure */
  pDMAch->CControl = 0x00;
  pDMAch->CConfig  = 0x00;

  /* Assign Linker List Item value */
  pDMAch->CLLI = 0;

#if 0
	if(chan == GPDMA_UART_0_TX_CHANNEL || chan == GPDMA_UART_2_TX_CHANNEL ||
			chan == GPDMA_UART_3_TX_CHANNEL){
		ccontrol = 0x86000000;
	}
	else if (chan == GPDMA_UART_0_RX_CHANNEL || chan == GPDMA_UART_2_RX_CHANNEL ||
			chan == GPDMA_UART_3_RX_CHANNEL){
		ccontrol = 0x89000000;
	}
	else if (chan == GPDMA_SPI_0_TX_CHANNEL || chan == GPDMA_SPI_1_TX_CHANNEL) {
		ccontrol = 0x86009000;
	} else if(chan == GPDMA_SPI_0_RX_CHANNEL || chan == GPDMA_SPI_1_RX_CHANNEL) {
		ccontrol = 0x89009000;
	}
#endif
  pDMAch->CSrcAddr  = desc->srcaddr;
  pDMAch->CDestAddr = desc->dstaddr;
  if (desc->srcperiph != 0)
  {
    pDMAch->CControl = desc->ccontrol | (desc->length & 0xFFF);  //0x89009000 | (desc->length & 0xFFF);
    SrcPeripheral    = desc->srcperiph;
    TransferType     = GPDMA_TRANSFERTYPE_P2M_CONTROLLER_DMA;
  }
  else if (desc->dstperiph != 0)
  {
    pDMAch->CControl = desc->ccontrol | (desc->length & 0xFFF);  //0x86009000 | (desc->length & 0xFFF);
    DestPeripheral   = desc->dstperiph;
    TransferType     = GPDMA_TRANSFERTYPE_M2P_CONTROLLER_DMA;
  }
  else
    return ERROR;

  /* Enable DMA channels, little endian */
  LPC_GPDMA->CONFIG = GPDMA_DMACConfig_E;
  while (!(LPC_GPDMA->CONFIG & GPDMA_DMACConfig_E))
    ;

  // Configure DMA Channel, enable Error Counter and Terminate counter
  pDMAch->CConfig = GPDMA_DMACCxConfig_E | GPDMA_DMACCxConfig_IE
      | GPDMA_DMACCxConfig_ITC
      | GPDMA_DMACCxConfig_TransferType(TransferType)
      | GPDMA_DMACCxConfig_SrcPeripheral(SrcPeripheral)
      | GPDMA_DMACCxConfig_DestPeripheral(DestPeripheral);

  NL_GPDMA_Callbacks[chan] = callback;

  return SUCCESS;
}

uint32_t NL_GPDMA_ChannelBusy(uint8_t ch)
{
  if (LPC_GPDMA->ENBLDCHNS & (GPDMA_DMACEnbldChns_Ch(ch)))
    /* This channel is already enabled */
    return 1;
  else
    return 0;
}
