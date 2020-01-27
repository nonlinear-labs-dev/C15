/******************************************************************************/
/** @file		nl_spi_bb.c
    @date		2015-03-12 DTZ
    @brief    	SPI to BeagleBone communication layer for LPC18xx
    @example
    @ingroup  	SPI_BB
    @author		Nemanja Nikodijevic 2014-01-31
*******************************************************************************/

#include "spibb/nl_spi_bb.h"
#include "drv/nl_gpio.h"
#include "cmsis/lpc43xx_ssp.h"
#include "string.h"
#include "dbg/nl_assert.h"
#include "sys/nl_ticker.h"

static LPC_SSPn_Type*  BB_SSP;
static SPI_BB_PINS_T*  pins;
static MessageCallback SPI_BB_MsgCb         = NULL;
static uint8_t*        SPI_BB_CurrentBuffer = 0;
/** rx buffers */
static uint8_t SPI_BB_Buffers[SPI_BB_BUFFER_NUM][SPI_BB_BUFFER_SIZE];
static uint8_t SPI_BB_BufferState[SPI_BB_BUFFER_NUM];
/** tx buffers */
static uint8_t  tx_buff[2][SPI_BB_BUFFER_SIZE];
static uint16_t tx_buff_offset = 0;
static uint8_t  tx_cur_buff    = 0;
static uint8_t  tx_prq         = 0;

#define PACKAGE_ENCLOSURE 0xFF

typedef struct __attribute__((__packed__))
{
  uint8_t  pre;
  uint16_t size_in_bytes;
  uint8_t  post;
} raw_package_header_t;

typedef struct __attribute__((__packed__))
{
  uint16_t type;
  uint16_t length;
} msg_header_t;

typedef struct __attribute__((__packed__))
{
  msg_header_t header;
  uint16_t     values[1];
} msg_t;

#if 1

/// static char log[128][32] = {};		/// Test für verloren gegangene Messages
/// static int logIdx = 0;

// calls the initialized callback for every message
static void SPI_BB_PackageParser(uint8_t* buff, uint32_t len)
{
  /// memcpy(log[logIdx % 128], buff, MIN(len, 32));	/// Test für verloren gegangene Messages
  /// logIdx++;

  uint32_t i;

  raw_package_header_t* rawPackage = (raw_package_header_t*) buff;
  uint32_t              todo       = rawPackage->size_in_bytes;

  if ((rawPackage->post == PACKAGE_ENCLOSURE) && (rawPackage->pre == PACKAGE_ENCLOSURE) && (todo <= (len - 4)))
  {
    buff += 4;

    for (i = 0; i < todo; i++)
    {
      msg_t* package = (msg_t*) buff;

      if (SPI_BB_MsgCb != NULL)
      {
        uint32_t time = SYS_ticker;
        SPI_BB_MsgCb(package->header.type, package->header.length, package->values);
        time = SYS_ticker - time;
        if (time > 1)
          time = 0;  // for breakpoint
      }

      uint32_t done = sizeof(msg_header_t) + sizeof(uint16_t) * package->header.length;

      i += done;
      buff += done;
    }
  }

  //  when the parsing for the buffer is finished
  for (i = 0; i < SPI_BB_BUFFER_NUM; i++)
  {
    if (rawPackage == (raw_package_header_t*) SPI_BB_Buffers[i])  /// Suche nicht nötig !!!
    {
      SPI_BB_BufferState[i] = SPI_BB_BUFFER_FREE;
      break;
    }
  }
}
#endif

#if 0  // nni original 2014-12-18 =>

static void SPI_BB_PackageParser(uint8_t* buff, uint32_t len)
{
	uint32_t i;
	uint16_t package_size;
	uint32_t* package = (uint32_t*) buff;
	uint16_t msg_type, msg_length;
	uint32_t* msg_data;

	if ((buff[0] & buff[3]) == 0xFF)
	{
		package_size = (package[0] & 0x00FFFF00) >> 8;

		for (i = 1; i <= package_size; i++)
		{
			msg_type = (uint16_t) (package[i] >> 16);
			msg_length = (uint16_t) (package[i] & 0xFFFF);

			if (msg_type == MSG_NOTIFICATION)
			{
				msg_data = NULL;
			}
			else
			{
				msg_data = package + i + 1;
				i += msg_length;
			}

			if (SPI_BB_MsgCb != NULL)
			{
				SPI_BB_MsgCb(msg_type, msg_length, msg_data);
			}
		}
	}

	//  when the parsing for the buffer is finished

	for (i = 0; i < SPI_BB_BUFFER_NUM; i++)
	{
		if (buff == (uint8_t*)SPI_BB_Buffers[i])
		{
			SPI_BB_BufferState[i] = SPI_BB_BUFFER_FREE;
			break;
		}
	}
}

#endif

static void SPI_BB_InitTxBuff(void)
{
  uint8_t               i;
  raw_package_header_t* rawPackage;

  for (i = 0; i < 2; i++)
  {
    rawPackage      = (raw_package_header_t*) tx_buff[i];
    rawPackage->pre = rawPackage->post = PACKAGE_ENCLOSURE;
    rawPackage->size_in_bytes          = 0;
  }

  tx_buff_offset = sizeof(raw_package_header_t);
}

static void SPI_BB_SendTxBuff(void)
{
  raw_package_header_t* rawPackage = (raw_package_header_t*) tx_buff[tx_cur_buff];

  rawPackage->size_in_bytes = tx_buff_offset - sizeof(raw_package_header_t);
  rawPackage->pre = rawPackage->post = PACKAGE_ENCLOSURE;
  SPI_DMA_Send(BB_SSP, tx_buff[tx_cur_buff], SPI_BB_BUFFER_SIZE, NULL);
  if (tx_buff_offset == sizeof(raw_package_header_t))
    tx_prq = 0;
  else
    tx_prq = 1;

  tx_cur_buff    = (tx_cur_buff + 1) % 2;
  tx_buff_offset = sizeof(raw_package_header_t);

  rawPackage                = (raw_package_header_t*) tx_buff[tx_cur_buff];
  rawPackage->size_in_bytes = 0;
}

static uint8_t* SPI_BB_RxBufferAvailable(void)
{
  uint8_t i;
  for (i = 0; i < SPI_BB_BUFFER_NUM; i++)
  {
    if (SPI_BB_BufferState[i] == SPI_BB_BUFFER_FREE)
      return (uint8_t*) SPI_BB_Buffers[i];
  }
  return NULL;
}

static void SPI_BB_ReceiveCallback(uint32_t ret)
{
  if (ret == SUCCESS)
  {
    uint32_t time = SYS_ticker;
    SPI_BB_PackageParser(SPI_BB_CurrentBuffer, SPI_BB_BUFFER_SIZE);
    time = SYS_ticker - time;
    if (time > 1)
      time = 0;  // for breakpoint
  }
}

static uint32_t SPI_BB_CheckGpIN(GPIO_NAME_T* gpio)
{
  if (LPC_GPIO_PORT->PIN[gpio->port] & (1 << gpio->pin))
    return 1;
  else
    return 0;
}

static uint32_t SPI_BB_CheckGpOUT(GPIO_NAME_T* gpio)
{
  if (LPC_GPIO_PORT->SET[gpio->port] & (1 << gpio->pin))
    return 1;
  else
    return 0;
}

/**********************************************************************
 * @brief		Configures the SPI-BB layer for the desired SSP
 * @param[in]	SSPx	Pointer to selected SSP peripheral, should be:
 * 					- LPC_SSP0	:SSP0 peripheral
 * 					- LPC_SSP1	:SSP1 peripheral
 * @param[in]	bb_pins	GPIO configuration structure pointer
 **********************************************************************/
void SPI_BB_Config(LPC_SSPn_Type* SSPx, SPI_BB_PINS_T* bb_pins)
{
  BB_SSP = SSPx;
  pins   = bb_pins;
}

/**********************************************************************
 * @brief		Initializes the SPI-BB communication
 * @param[in]	msg_cb	Pointer to the MessageCallback function
 **********************************************************************/
void SPI_BB_Init(MessageCallback msg_cb)
{
  SPI_BB_MsgCb = msg_cb;
  SPI_BB_Reset();
}

/**********************************************************************
 * @brief   Resets the SPI-BB communication
 **********************************************************************/
void SPI_BB_Reset(void)
{
  SPI_BB_InitTxBuff();
  SPI_DMA_Init(BB_SSP, SSP_SLAVE_MODE, 100000);
}

/**********************************************************************
 * @brief		SPI-BB polling routine
 **********************************************************************/

void SPI_BB_Polling(void)
{
  uint8_t* rcv_buff;
  uint8_t  i;

  if (SPI_BB_CheckGpOUT(pins->heartbeat))
  {
    NL_GPIO_Clr(pins->heartbeat);
  }
  else
  {
    NL_GPIO_Set(pins->heartbeat);
  }

  if ((tx_prq || (tx_buff_offset > sizeof(raw_package_header_t))) && !SPI_BB_CheckGpOUT(pins->prq))
    NL_GPIO_Set(pins->prq);
  else if (!(tx_prq || (tx_buff_offset > sizeof(raw_package_header_t))) && SPI_BB_CheckGpOUT(pins->prq))
    NL_GPIO_Clr(pins->prq);

  /** @todo recovery in case CS-low but no SPI-transfer */
  /* Chip select went down? */
  if (!SPI_BB_CheckGpIN(pins->cs) && SPI_BB_CheckGpOUT(pins->rdy))
    NL_GPIO_Clr(pins->rdy);
  else if (SPI_BB_CheckGpIN(pins->cs) && !SPI_BB_CheckGpOUT(pins->rdy))
  {
    /* Chip select went up? */
    rcv_buff = SPI_BB_RxBufferAvailable();
    if (rcv_buff)
    {
      if (SPI_DMA_Receive(BB_SSP, rcv_buff, SPI_BB_BUFFER_SIZE, (TransferCallback) SPI_BB_ReceiveCallback))
      {
        //*********************************
        SPI_BB_SendTxBuff();

        SPI_BB_CurrentBuffer = rcv_buff;
        NL_GPIO_Set(pins->rdy);
        for (i = 0; i < SPI_BB_BUFFER_NUM; i++)
          if (rcv_buff == (uint8_t*) SPI_BB_Buffers[i])
          {
            SPI_BB_BufferState[i] = SPI_BB_BUFFER_BUSY;  /// Suche nicht nötig !!!
            break;
          }
      }
    }
  }
}

/**********************************************************************
 * @param[in]	buff	Pointer to the buffer
 * @param[in]	len		Length of the buffer
 * @return		len - Success; 0 - failure
 **********************************************************************/

uint32_t SPI_BB_Send(uint8_t* buff, uint32_t len)
{
  uint32_t i;

  if ((tx_buff_offset + len) >= SPI_BB_BUFFER_SIZE)
  {
    return 0;
  }

  for (i = 0; i < len; i++)
  {
    tx_buff[tx_cur_buff][tx_buff_offset++] = buff[i];
  }

  return len;
}

/******************************************************************************/
/**	param[in]	state of prq, cs, heartbeat, rdy
				-  0: gpios off
 	 	 	 	- >0: gpios on
*******************************************************************************/

void SPI_BB_TestGpios(uint8_t state)
{
  if (state == 0)
  {
    NL_GPIO_Clr(pins->prq);
    NL_GPIO_Clr(pins->cs);
    NL_GPIO_Clr(pins->heartbeat);
    NL_GPIO_Clr(pins->rdy);
  }
  else
  {
    NL_GPIO_Set(pins->prq);
    NL_GPIO_Set(pins->cs);
    NL_GPIO_Set(pins->heartbeat);
    NL_GPIO_Set(pins->rdy);
  }
}
