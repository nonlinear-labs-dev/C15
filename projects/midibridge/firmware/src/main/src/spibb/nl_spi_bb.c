/******************************************************************************/
/** @file		nl_spi_bb.c
    @date		2015-03-12 DTZ
    @brief    	SPI to BeagleBone communication layer for LPC18xx
    @example
    @ingroup  	SPI_BB
    @author		Nemanja Nikodijevic 2014-01-31
*******************************************************************************/

#include "spibb/nl_spi_bb.h"
#include "io/pins.h"
#include "cmsis/lpc43xx_ssp.h"

static LPC_SSPn_Type*  BB_SSP               = LPC_SSP1;
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

static int enable_heartbeat = 0;

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
  uint16_t     values[512];
} msg_t;

// calls the initialized callback for every message
static void SPI_BB_PackageParser(uint8_t* buff, uint32_t len)
{
  /// memcpy(log[logIdx % 128], buff, MIN(len, 32));	/// Test für verloren gegangene Messages
  /// logIdx++;

  uint32_t i;

  raw_package_header_t* rawPackage = (raw_package_header_t*) buff;
  uint32_t              todo       = rawPackage->size_in_bytes;

  if (!buff || !*((uint64_t*) buff))  // error condition empty buffer
    while (1)                         // looping here ...
      ;                               // will trigger watchdog reset
#warning "TODO: Find root cause of empty DMA packets problem"

  if ((rawPackage->post == PACKAGE_ENCLOSURE) && (rawPackage->pre == PACKAGE_ENCLOSURE) && (todo <= (len - 4)))
  {
    buff += 4;

    for (i = 0; i < todo;)
    {
      msg_t* package = (msg_t*) buff;

      if (SPI_BB_MsgCb != NULL)
      {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#pragma GCC diagnostic ignored "-Wattributes"
        SPI_BB_MsgCb(package->header.type, package->header.length, package->values);
#pragma GCC diagnostic push
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
    SPI_BB_PackageParser(SPI_BB_CurrentBuffer, SPI_BB_BUFFER_SIZE);
  }
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
  // clock rate is irrelevant in slave mode, but let's just use the
  // 6MHz set up by the master (playcontroller_driver.c on the BBB) for consistency
  SPI_DMA_Init(BB_SSP, SSP_SLAVE_MODE, 6000000);
}

/**********************************************************************
 * @brief		toggle heartbeat
 **********************************************************************/
void SPI_BB_ToggleHeartbeat(void)
{
  if (!enable_heartbeat)
    return;
  pinBspi_HBTb = !pinBspi_HBTb;
}

/**********************************************************************
 * @brief		SPI-BB polling routine
 **********************************************************************/
void SPI_BB_Polling(void)
{
  uint8_t* rcv_buff;
  uint8_t  i;

  enable_heartbeat = 1;

  if (!pinBspi_PRQb && (tx_prq || (tx_buff_offset > sizeof(raw_package_header_t))))
    pinBspi_PRQb = 1;
  else if (pinBspi_PRQb && !(tx_prq || (tx_buff_offset > sizeof(raw_package_header_t))))
    pinBspi_PRQb = 0;

  /** @todo recovery in case CS-low but no SPI-transfer */
  /* Chip select went down? */
  if (!pinBspi_SCSb && pinBspi_RDYb)
    pinBspi_RDYb = 0;
  else if (pinBspi_SCSb && !pinBspi_RDYb)
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
        pinBspi_RDYb         = 1;
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
