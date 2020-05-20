/******************************************************************************/
/** @file		nl_usb_audio.c
    @date		2015-08-10
    @brief    	USB Audio device driver for LPC43xx
    @author		Nemanja Nikodijevic 2015-08-10
*******************************************************************************/
#include <stdint.h>
#include "cmsis/LPC43xx.h"
#include "usb/nl_usb_audio.h"
#include "usb/nl_usb_descaudio.h"
#include "usb/nl_usb_core.h"
#include "usb/nl_usbd.h"

#define AUDIO_VOLUME_MAX 0xFFF0
#define AUDIO_VOLUME_MIN 0xE3A0
#define AUDIO_VOLUME_RES 0x0030

#define USB_AUDIO_OUTBUFF_SIZE USB_AUDIO_ISO_MAX_PKT
static uint8_t outbuff[USB_AUDIO_OUTBUFF_SIZE];
//static uint8_t inbuff[USB_AUDIO_OUTBUFF_SIZE];
static uint8_t tmpoutbuff[USB_AUDIO_OUTBUFF_SIZE * 2];

static uint32_t sampleFreq = 48000;
static uint16_t volumeOut  = 0xFFC0;
static uint8_t  muteOut    = 0;
static uint32_t tmpVol     = 0;

static uint8_t outInterfaceOn = 0;

static AudioRcvCallback Audio_Callback_OUT = 0;
static LPC_TIMERn_Type* USB_Timer          = 0;

static void USB_Timer_Init(void)
{
  USB_Timer->CCR &= ~(0x03);
  USB_Timer->CCR |= 1; /* timer mode - capture on rising edge */

  USB_Timer->TC = 0;
  USB_Timer->PC = 0;
  USB_Timer->PR = 0;

  /* reset TC */
  USB_Timer->TCR |= (1 << 1);
  USB_Timer->TCR &= ~(1 << 1);
}

static void USB_Timer_Start(void)
{
  /* reset TC */
  USB_Timer->TCR = 0;
  USB_Timer->TCR |= (1 << 1);
  USB_Timer->TCR &= ~(1 << 1);
  USB_Timer->TC  = 0;
  USB_Timer->TCR = 1;
}

static void USB_Audio_SOF(void)
{
  static uint32_t feedback = 0xBFFD5;  //(203989<<14)/4250;//48<<14;
  uint32_t        timer_val;

  if (outInterfaceOn)
  {
    if (outInterfaceOn == 2)
    {
      USB_Timer_Start();
      outInterfaceOn = 1;
    }
    else
    {
      timer_val = USB_Timer->TC;
      feedback  = timer_val << 14 / 8500;  //204000000 / 4250 = 48000
      USB_Timer_Start();
    }
    USB_WriteEP(0x81, (uint8_t*) &feedback, 3);
  }
}

/******************************************************************************/
/** @brief		Endpoint 1 Callback
    @param[in]	event	Event that triggered the interrupt
*******************************************************************************/
static void USB_EndPoint1(uint32_t event)
{

  uint32_t length;
  uint16_t i, j;

  switch (event)
  {
    case USB_EVT_OUT:
      length = USB_ReadEP(0x01, outbuff);
      if (length)
      {
        for (i = 0, j = 0; i < length; i += 3, j += 4)
        {
          tmpoutbuff[j]     = 0;
          tmpoutbuff[j + 1] = outbuff[i];
          tmpoutbuff[j + 2] = outbuff[i + 1];
          tmpoutbuff[j + 3] = outbuff[i + 2];
        }
        if (Audio_Callback_OUT)
          Audio_Callback_OUT((uint32_t*) tmpoutbuff, j / 4);
      }
      USB_ReadReqEP(0x01, outbuff, USB_AUDIO_OUTBUFF_SIZE);
      break;
  }
}

/******************************************************************************/
/** @brief		USB Interface Event handler, happens when interface ALT setting
 * 				changes
    @param[in]	spkt	Setup packet from the control endpoint
*******************************************************************************/
static void USB_Audio_Interface_Event(USB_SETUP_PACKET* spkt)
{
  static uint16_t wIndex = 0;  //spkt->wIndex.W;
  static uint16_t wValue = 0;  //spkt->wValue.W;
  wIndex                 = spkt->wIndex.W;
  wValue                 = spkt->wValue.W;

  if (wIndex == USB_AUDIO_INPUT_INTERFACE)  //interface 1: streaming IN - line in (EP 2)
  {
    if (wValue == 0x0001)
    {
    }
    else
    {
      USB_ResetEP(0x82);
    }
  }
  else if (wIndex == USB_AUDIO_OUTPUT_INTERFACE)  //interface 2: streaming OUT - speaker (EP 1)
  {
    if (wValue == 0x0001)
    {
      USB_ReadReqEP(0x01, outbuff, USB_AUDIO_OUTBUFF_SIZE);
      outInterfaceOn = 2;
    }
    else
    {
      USB_ResetEP(0x01);
      outInterfaceOn = 0;
    }
  }
}

static void USB_Audio_IF_GetRequest(USB_SETUP_PACKET* spkt, USB_EP_DATA* ep0dat)
{

  switch (spkt->wValue.WB.H)
  {
    case USB_AUDIO_MUTE_CTRL:
      switch (spkt->bRequest)
      {
        case USB_AUDIO_REQ_GET_CUR:
          if (spkt->wIndex.W == 0x0500) /** Feature Unit Audio OUT */
            ep0dat->pData = (uint8_t*) &muteOut;
          break;
        default:
          return;
      };
      break;
    case USB_AUDIO_VOLUME_CTRL:
      switch (spkt->bRequest)
      {
        case USB_AUDIO_REQ_GET_CUR:
          if (spkt->wIndex.W == 0x0500) /** Feature Unit Audio OUT */
            ep0dat->pData = (uint8_t*) &volumeOut;
          break;
        case USB_AUDIO_REQ_GET_MIN:
          tmpVol        = AUDIO_VOLUME_MIN;
          ep0dat->pData = (uint8_t*) &tmpVol;
          break;
        case USB_AUDIO_REQ_GET_MAX:
          tmpVol        = AUDIO_VOLUME_MAX;
          ep0dat->pData = (uint8_t*) &tmpVol;
          break;
        case USB_AUDIO_REQ_GET_RES:
          tmpVol        = AUDIO_VOLUME_RES;
          ep0dat->pData = (uint8_t*) &tmpVol;
          break;
      }
  };
}

static uint8_t USB_Audio_Class_Request(USB_SETUP_PACKET* spkt, USB_EP_DATA* ep0dat, uint32_t event)
{
  switch (spkt->bmRequestType.BM.Recipient)
  {
    case REQUEST_TO_INTERFACE:
      switch (spkt->bRequest)
      {
        case USB_AUDIO_REQ_GET_CUR:
        case USB_AUDIO_REQ_GET_MIN:
        case USB_AUDIO_REQ_GET_MAX:
        case USB_AUDIO_REQ_GET_RES:
          if (event != USB_EVT_SETUP)
            return 0;
          USB_Audio_IF_GetRequest(spkt, ep0dat);
          USB_DataInStage();
          return 0;

        case USB_AUDIO_REQ_SET_CUR:
        case USB_AUDIO_REQ_SET_MIN:
        case USB_AUDIO_REQ_SET_MAX:
        case USB_AUDIO_REQ_SET_RES:
          if (event != USB_EVT_OUT)
            return 0;
          USB_StatusInStage();
          return 0;
        default:
          return 1;
      };
      break;

    case REQUEST_TO_ENDPOINT:
      switch (spkt->bRequest)
      {
        case USB_AUDIO_REQ_SET_CUR:
          if (event != USB_EVT_OUT)
            return 0;
          if ((spkt->wValue.W == 0x0100) && (spkt->wIndex.W == 0x0001) && (spkt->wLength = 0x0003))
          {
            /** Set Sampling Frequency - OUT EP1 */
            USB_StatusInStage();
            return 0;
          }
          else if ((spkt->wValue.W == 0x0100) && (spkt->wIndex.W == 0x0082) && (spkt->wLength = 0x0003))
          {
            /** Set Sampling Frequency - IN EP2 */
            USB_StatusInStage();
            return 0;
          }
          else
            return 1;
          break;
        case USB_AUDIO_REQ_GET_CUR:
          if (event != USB_EVT_SETUP)
            return 0;
          if ((spkt->wValue.W == 0x0100) && (spkt->wIndex.W == 0x0001))
          {
            /** Get Sampling Frequency - OUT EP1 */
            ep0dat->pData = (uint8_t*) &sampleFreq;
            USB_DataInStage();
            return 0;
          }
          else if ((spkt->wValue.W == 0x0100) && (spkt->wIndex.W == 0x0082))
          {
            /** Get Sampling Frequency - IN EP2 */
            ep0dat->pData = (uint8_t*) &sampleFreq;
            USB_DataInStage();
            return 0;
          }
          else
            return 1;
          break;
        default:
          return 1;
      };
      break;
    default:
      return 0;
  }
}

/******************************************************************************/
/** @brief    Function that initializes USB Audio driver for USB0 controller
*******************************************************************************/
void USB_Audio_Init(AudioRcvCallback audrcv, LPC_TIMERn_Type* TIMx)
{
  Audio_Callback_OUT = audrcv;
  USB_Timer          = TIMx;
  /** assign descriptors */
  USB_Core_Device_Descriptor_Set((const uint8_t*) USB_Audio_DeviceDescriptor);
  USB_Core_Device_FS_Descriptor_Set((const uint8_t*) USB_Audio_FSConfigDescriptor);
  USB_Core_Device_HS_Descriptor_Set((const uint8_t*) USB_Audio_HSConfigDescriptor);
  USB_Core_Device_String_Descriptor_Set((const uint8_t*) USB_Audio_StringDescriptor);
  USB_Core_Device_Device_Quali_Descriptor_Set((const uint8_t*) USB_Audio_DeviceQualifier);
  /** @todo: assign callbacks */
  USB_Core_Interface_Event_Handler_Set(USB_Audio_Interface_Event);
  USB_Core_Class_Request_Handler_Set(USB_Audio_Class_Request);
  USB_Core_Endpoint_Callback_Set(1, USB_EndPoint1);
  USB_Core_SOF_Event_Handler_Set(USB_Audio_SOF);
  USB_Core_Init();
  USB_Core_ForceFullSpeed();
  USB_Timer_Init();
  NVIC_EnableIRQ(USB0_IRQn);
}

/******************************************************************************/
/** @brief    Function for polling the USB Audio driver
*******************************************************************************/
void USB_Audio_Poll(void)
{
  USB0_IRQHandler();
}
