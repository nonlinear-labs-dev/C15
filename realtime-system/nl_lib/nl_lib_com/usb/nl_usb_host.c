/******************************************************************************/
/** @file		nl_usb_host.c
    @date		2015-10-15
    @brief    	Driver for the USB host port on LPC43xx
    @author		Nemanja Nikodijevic 2015-10-15
*******************************************************************************/
#include "usb/nl_usb_host.h"
#include <stdlib.h>

#ifdef USB_MIDI_HOST
#include "usb/nl_usb_host_midi.h"
#endif

#define ALIGN32(x) (x & ~(0x1F))

static uint8_t  USBH_Connected       = 0;
static uint8_t  USBH_DeviceSpeed     = 0;
static uint16_t USBH_Config_Desc_Len = 0;

typedef enum USBH_Enum_Phases
{
  USBH_Enum_None               = 0,
  USBH_Enum_Connected          = 1,
  USBH_Enum_Reset              = 2,
  USBH_Enum_Get_Descriptors    = 3,
  USBH_Enum_Set_Address        = 4,
  USBH_Enum_Get_Configuration  = 5,
  USBH_Enum_Get_Configuration1 = 6,
  USBH_Enum_Set_Configuration  = 7,
  USBH_Enum_DeEnumerate        = 8,
} USBH_Enum_Phase;

static USBH_Enum_Phase USBH_Enumeration = USBH_Enum_None;

/** Control transfer structures */
static QH_T    ctrl_QH;
static QTD_T   ctrl_TD[3];
static uint8_t ctrlBuffer[8];
static uint8_t descBuffer[128];

static void USB_Host_Ctrl_QH_Init(void)
{
  ctrl_QH.ep_chars = USBH_QH_MAX_PKT(8) | USBH_QH_DTC | (USBH_DeviceSpeed << 12) | (1 << 15);
  if (USBH_DeviceSpeed != USBH_HIGH_SPEED)
    ctrl_QH.ep_chars |= USBH_QH_CTRL_EP_FLAG;
  ctrl_QH.ep_caps      = USBH_QH_MULT(1);
  ctrl_QH.curr_qTD     = 0;
  ctrl_QH.next_qTD     = USBH_LINK_TERMINATOR;
  ctrl_QH.alt_next_qTD = USBH_LINK_TERMINATOR;
  ctrl_QH.total_bytes  = 0;
  ctrl_QH.buffer0 = ctrl_QH.buffer1 = ctrl_QH.buffer2 = 0;
  ctrl_QH.buffer3 = ctrl_QH.buffer4 = 0;
}

static void USB_Host_Ctrl_QH_Set_Max_Packet_Size(uint8_t size)
{
  ctrl_QH.ep_chars &= ~(0x7FF << 16);
  ctrl_QH.ep_chars |= USBH_QH_MAX_PKT(size);
}

static void USB_Host_Ctrl_QH_Set_Device_Address(uint8_t addr)
{
  ctrl_QH.ep_chars |= addr & 0x7F;
}

/** by default dt = 0, ioc = 1 */
void USB_Host_PrepareTD(QTD_T* qTD, uint8_t dir, uint8_t* buffer, uint16_t length)
{
  qTD->next_qTD     = USBH_LINK_TERMINATOR;
  qTD->alt_next_qTD = USBH_LINK_TERMINATOR;
  qTD->total_bytes  = (dir << 8) | USBH_TD_IOC | (length << 16) | USBH_TD_STATUS_ACTIVE;
  qTD->buffer0      = (uint32_t) buffer;
  qTD->buffer1      = ((uint32_t) buffer & 0xFFFFF000) + 0x1000;
  qTD->buffer2      = ((uint32_t) buffer & 0xFFFFF000) + 0x2000;
  qTD->buffer3      = ((uint32_t) buffer & 0xFFFFF000) + 0x3000;
  qTD->buffer4      = ((uint32_t) buffer & 0xFFFFF000) + 0x4000;
}

static void USB_Host_Ctrl_Request(uint8_t reqType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, uint16_t wLength, uint8_t* buffer)
{
  ctrlBuffer[0]                   = reqType;
  ctrlBuffer[1]                   = bRequest;
  *((uint16_t*) (ctrlBuffer + 2)) = wValue;
  *((uint16_t*) (ctrlBuffer + 4)) = wIndex;
  *((uint16_t*) (ctrlBuffer + 6)) = wLength;

  USB_Host_PrepareTD(&ctrl_TD[0], USBH_DIR_SETUP, ctrlBuffer, 8);
  ctrl_TD[0].total_bytes &= ~USBH_TD_IOC;
  ctrl_TD[0].next_qTD = (uint32_t) &ctrl_TD[2];

  if (wLength)
  {
    USB_Host_PrepareTD(&ctrl_TD[1], (reqType & 0x80) ? USBH_DIR_IN : USBH_DIR_OUT, buffer, wLength);
    ctrl_TD[1].total_bytes |= USBH_TD_DATA_TOGGLE;
    ctrl_TD[1].total_bytes &= ~USBH_TD_IOC;

    ctrl_TD[0].next_qTD = (uint32_t) &ctrl_TD[1];
    ctrl_TD[1].next_qTD = (uint32_t) &ctrl_TD[2];
  }

  USB_Host_PrepareTD(&ctrl_TD[2], (reqType & 0x80) ? USBH_DIR_OUT : USBH_DIR_IN, NULL, 0);
  ctrl_TD[2].total_bytes |= USBH_TD_DATA_TOGGLE;

  ctrl_QH.next_qTD = (uint32_t) &ctrl_TD[0];
}

static void USB_Host_Enumerate(void)
{
  static uint32_t host_wait = 0;

  if (host_wait)
  {
    host_wait--;
    return;
  }

  switch (USBH_Enumeration)
  {
    case USBH_Enum_Connected:
      host_wait        = 200;
      USBH_Enumeration = USBH_Enum_Reset;
      break;
    case USBH_Enum_Reset:
      LPC_USB0->PORTSC1_H |= USBH_PORTSC_PR;
      while (LPC_USB0->PORTSC1_H & USBH_PORTSC_PR)
        ;
      USBH_DeviceSpeed = (uint8_t)((LPC_USB0->PORTSC1_H >> 26) & 0x3);
      host_wait        = 100;
      USBH_Enumeration = USBH_Enum_Get_Descriptors;
      break;
    case USBH_Enum_Get_Descriptors:
      USB_Host_Ctrl_QH_Init();
      USB_Host_Ctrl_Request(USBH_REQ_DIR_D2H, USBH_REQ_GET_DESCRIPTOR, (USBH_DESC_DEVICE << 8), 0, 8, descBuffer);
      USBH_Enumeration = USBH_Enum_Set_Address;
      break;
    case USBH_Enum_Set_Address:
      USB_Host_Ctrl_QH_Set_Max_Packet_Size(descBuffer[7]);
      USB_Host_Ctrl_Request(USBH_REQ_DIR_H2D, USBH_REQ_SET_ADDRESS, 1, 0, 0, NULL);
      USBH_Enumeration = USBH_Enum_Get_Configuration;
      break;
    case USBH_Enum_Get_Configuration:
      USB_Host_Ctrl_QH_Set_Device_Address(1);
      USB_Host_Ctrl_Request(USBH_REQ_DIR_D2H, USBH_REQ_GET_DESCRIPTOR, (USBH_DESC_CONFIGURATION << 8), 0, 9, descBuffer);
      USBH_Enumeration = USBH_Enum_Get_Configuration1;
      break;
    case USBH_Enum_Get_Configuration1:
      USBH_Config_Desc_Len = *((uint16_t*) (descBuffer + 2));
      USB_Host_Ctrl_Request(USBH_REQ_DIR_D2H, USBH_REQ_GET_DESCRIPTOR, (USBH_DESC_CONFIGURATION << 8), 0, USBH_Config_Desc_Len, descBuffer);
      USBH_Enumeration = USBH_Enum_Set_Configuration;
    case USBH_Enum_Set_Configuration:
#ifdef USB_MIDI_HOST
      for (host_wait = 55555; host_wait > 0; host_wait--)
        ;
      if (USBH_MIDI_Parse_Config(descBuffer, USBH_Config_Desc_Len))
      {
        USBH_MIDI_Prepare_Endpoints();
        USB_Host_Ctrl_Request(USBH_REQ_DIR_H2D, USBH_REQ_SET_CONFIGURATION, 1, 0, 0, NULL);
      }
#endif
      USBH_Enumeration = USBH_Enum_None;
      break;
    case USBH_Enum_DeEnumerate:
#ifdef USB_MIDI_HOST
      USBH_MIDI_Remove_Device();
#endif
      USBH_Enumeration = USBH_Enum_None;
      break;
    default:
      break;
  };
}

void USB_Host_Init(void)
{
  /* Turn on the phy */
  LPC_CREG->CREG0 &= ~(1 << 5);
  /* reset the controller */
  LPC_USB0->USBCMD_H |= USBH_USBCMD_RST;
  /* wait for reset to complete */
  while (LPC_USB0->USBCMD_H & USBH_USBCMD_RST)
    ;

  /* program the controller to be the USB host */
  /** @todo: VBPS? */
  LPC_USB0->USBMODE_H = USBH_USBMODE_CM_HOST | USBH_USBMODE_VBPS_HI;

  /** @todo: LPC_USB0->OTGSC = ?; */
  //LPC_USB0->OTGSC |= 1<<1;

  /* enable interrupts */
  LPC_USB0->USBINTR_H = USBH_USBSTS_UI | USBH_USBSTS_UEI | USBH_USBSTS_PCI | USBH_USBSTS_UAI;
  /** @todo: prepare the periodic list */
  /** prepare the Async list */
  ctrl_QH.next_QH = ((uint32_t) &ctrl_QH | 0x2);
  USB_Host_Ctrl_QH_Init();
  /* assign the pointer to the Async list */
  LPC_USB0->ASYNCLISTADDR = (uint32_t) &ctrl_QH;
  LPC_USB0->USBCMD_H |= USBH_USBCMD_ASE | USBH_USBCMD_RS;

  LPC_USB0->PORTSC1_H |= USBH_PORTSC_PP;

  NVIC_EnableIRQ(USB0_IRQn);
}

/******************************************************************************/
/** @brief		USB Host regular task, should be called every 1ms
*******************************************************************************/
void USB_Host_Task(void)
{
  /** check if device should be enumerated */
  if (USBH_Enumeration != USBH_Enum_None)
  {
    USB_Host_Enumerate();
  }
}

/******************************************************************************/
/** @brief		USB Interrupt Service Routine
*******************************************************************************/
void USB0_IRQHandler(void)
{
  uint32_t hisr;

  hisr               = LPC_USB0->USBSTS_H; /* Host Interrupt Status */
  LPC_USB0->USBSTS_H = hisr;

  if (hisr & USBH_USBSTS_PCI)
  {
    if (USBH_Enumeration != USBH_Enum_Reset)
    {
      if (USBH_Connected)
      {
        USBH_Connected   = 0;
        USBH_Enumeration = USBH_Enum_DeEnumerate;
      }
      else
      {
        USBH_Connected   = 1;
        USBH_Enumeration = USBH_Enum_Connected;
      }
    }
  }

  if (hisr & USBH_USBSTS_UAI)
  {
    if (USBH_Enumeration == USBH_Enum_None)
    {
#ifdef USB_MIDI_HOST
      USBH_MIDI_QH_Process();
#endif
    }
  }

  return;
}

void USB_Host_QH_Init(QH_T* qh, uint8_t epnum, uint16_t maxpkt)
{
  qh->ep_chars     = USBH_QH_EP_NUM(epnum) | USBH_QH_MAX_PKT(maxpkt) | (USBH_DeviceSpeed << 12) | 1;
  qh->ep_caps      = USBH_QH_MULT(1);
  qh->curr_qTD     = 0;
  qh->next_qTD     = USBH_LINK_TERMINATOR;
  qh->alt_next_qTD = USBH_LINK_TERMINATOR;
  qh->total_bytes  = 0;
  qh->buffer0      = 0;
  qh->buffer1      = 0;
  qh->buffer2      = 0;
  qh->buffer3      = 0;
  qh->buffer4      = 0;
}

void USB_Host_QH_Insert(QH_T* qh)
{
  LPC_USB0->USBCMD_H &= ~USBH_USBCMD_ASE;
  while (LPC_USB0->USBSTS_H & USBH_USBSTS_AS)
    ;

  qh->next_QH     = ctrl_QH.next_QH;
  ctrl_QH.next_QH = (uint32_t) qh | 0x2;

  LPC_USB0->USBCMD_H |= USBH_USBCMD_ASE;
  while (!(LPC_USB0->USBSTS_H & USBH_USBSTS_AS))
    ;
}

void USB_Host_QH_Remove(QH_T* qh)
{
  QH_T*    pQH       = (QH_T*) &ctrl_QH;
  uint32_t qh_remove = (uint32_t) qh;

  while ((ALIGN32(pQH->next_QH) != (uint32_t) &ctrl_QH) && (ALIGN32(pQH->next_QH) != qh_remove))
    pQH = (QH_T*) ALIGN32(pQH->next_QH);

  if (ALIGN32(pQH->next_QH) == qh_remove)
  {
    pQH->next_QH = qh->next_QH;
  }
}
