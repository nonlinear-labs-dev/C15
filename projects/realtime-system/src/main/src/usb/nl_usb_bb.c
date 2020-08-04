/******************************************************************************/
/** @file		nl_usb_bb.h
    @date		2020-08-04
    @brief    	Control interface USB to/from PG (ePC) via MIDI
    @example
    @ingroup  	USB_BB
    @author		KSTR
*******************************************************************************/

#include "usb/nl_usb_bb.h"
#include "cmsis/lpc_types.h"
#include <stddef.h>

static MessageCallback USB_BB_MsgCb         = NULL;
static uint8_t*        USB_BB_CurrentBuffer = 0;
/** rx buffers */
static uint8_t USB_BB_Buffers[USB_BB_BUFFER_NUM][USB_BB_BUFFER_SIZE];
static uint8_t USB_BB_BufferState[USB_BB_BUFFER_NUM];
/** tx buffers */
static uint8_t  tx_buff[2][USB_BB_BUFFER_SIZE];
static uint16_t tx_buff_offset = 0;
static uint8_t  tx_cur_buff    = 0;
static uint8_t  tx_prq         = 0;

#define PACKAGE_ENCLOSURE  0xFF
#define USB_BB_BUFFER_FREE 0
#define USB_BB_BUFFER_BUSY 1

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
static void USB_BB_PackageParser(uint8_t* buff, uint32_t len)
{
  uint32_t i;

  raw_package_header_t* rawPackage = (raw_package_header_t*) buff;
  uint32_t              todo       = rawPackage->size_in_bytes;

#if 0
  if (!buff || !*((uint64_t*) buff))  // error condition empty buffer
    while (1)                         // looping here ...
      ;                               // will trigger watchdog reset
#warning "TODO: Find root cause of empty DMA packets problem"
#endif

  if ((rawPackage->post == PACKAGE_ENCLOSURE) && (rawPackage->pre == PACKAGE_ENCLOSURE) && (todo <= (len - 4)))
  {
    buff += 4;

    for (i = 0; i < todo;)
    {
      msg_t* package = (msg_t*) buff;

      if (USB_BB_MsgCb != NULL)
      {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#pragma GCC diagnostic ignored "-Wattributes"
        USB_BB_MsgCb(package->header.type, package->header.length, package->values);
#pragma GCC diagnostic push
      }

      uint32_t done = sizeof(msg_header_t) + sizeof(uint16_t) * package->header.length;

      i += done;
      buff += done;
    }
  }

  //  when the parsing for the buffer is finished
  for (i = 0; i < USB_BB_BUFFER_NUM; i++)
  {
    if (rawPackage == (raw_package_header_t*) USB_BB_Buffers[i])  /// Suche nicht nötig !!!
    {
      USB_BB_BufferState[i] = USB_BB_BUFFER_FREE;
      break;
    }
  }
}

static void USB_BB_InitTxBuff(void)
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

static void USB_BB_SendTxBuff(void)
{
  raw_package_header_t* rawPackage = (raw_package_header_t*) tx_buff[tx_cur_buff];

  rawPackage->size_in_bytes = tx_buff_offset - sizeof(raw_package_header_t);
  rawPackage->pre = rawPackage->post = PACKAGE_ENCLOSURE;
  // TODO : replace SPI_DMA_Send(BB_SSP, tx_buff[tx_cur_buff], USB_BB_BUFFER_SIZE, NULL);  with MIDI wrapped BB msg
  if (tx_buff_offset == sizeof(raw_package_header_t))
    tx_prq = 0;
  else
    tx_prq = 1;

  tx_cur_buff    = (tx_cur_buff + 1) % 2;
  tx_buff_offset = sizeof(raw_package_header_t);

  rawPackage                = (raw_package_header_t*) tx_buff[tx_cur_buff];
  rawPackage->size_in_bytes = 0;
}

static uint8_t* USB_BB_RxBufferAvailable(void)
{
  uint8_t i;
  for (i = 0; i < USB_BB_BUFFER_NUM; i++)
  {
    if (USB_BB_BufferState[i] == USB_BB_BUFFER_FREE)
      return (uint8_t*) USB_BB_Buffers[i];
  }
  return NULL;
}

static void USB_BB_ReceiveCallback(uint32_t ret)
{
  if (ret == SUCCESS)
  {
    USB_BB_PackageParser(USB_BB_CurrentBuffer, USB_BB_BUFFER_SIZE);
  }
}

/**********************************************************************
 * @brief		Initializes the SPI-BB communication
 * @param[in]	msg_cb	Pointer to the MessageCallback function
 **********************************************************************/
void USB_BB_Init(MessageCallback msg_cb)
{
  USB_BB_MsgCb = msg_cb;
  USB_BB_Reset();
}

/**********************************************************************
 * @brief   Resets the SPI-BB communication
 **********************************************************************/
void USB_BB_Reset(void)
{
  USB_BB_InitTxBuff();
  // clock rate is irrelevant in slave mode, but let's just use the
  // 6MHz set up by the master (lpc_bb_driver.c on the BBB) for consistency
}

/**********************************************************************
 * @param[in]	buff	Pointer to the buffer
 * @param[in]	len		Length of the buffer
 * @return		len - Success; 0 - failure
 **********************************************************************/

uint32_t USB_BB_Send(uint8_t* buff, uint32_t len)
{
  uint32_t i;

  if ((tx_buff_offset + len) >= USB_BB_BUFFER_SIZE)
    return 0;

  for (i = 0; i < len; i++)
    tx_buff[tx_cur_buff][tx_buff_offset++] = buff[i];

  USB_BB_SendTxBuff();

  return len;
}
