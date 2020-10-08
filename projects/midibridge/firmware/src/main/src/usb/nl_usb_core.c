/******************************************************************************/
/** @file		nl_usb_core.c
    @date		2014-12-11
    @brief    	Core functionality for the USB
    @example
    @ingroup	nl_drv_modules
    @author		Nemanja Nikodijevic 2014-12-11
*******************************************************************************/
#include "cmsis/LPC43xx.h"
#include "cmsis/lpc_types.h"
#include "usb/nl_usbd.h"
#include "usb/nl_usb_core.h"
#include "sys/nl_stdlib.h"

#ifdef __CC_ARM
#pragma diag_suppress 111, 177, 1441
#endif

#if defined(__GNUC__)
#define __packed __attribute__((__packed__))
#endif

#ifdef __ICCARM__
#pragma data_alignment = 2048
DQH_T ep_QH[EP_NUM_MAX];
#pragma data_alignment = 32
DTD_T ep_TD[EP_NUM_MAX];
#pragma data_alignment = 4
#elif defined(__GNUC__)
#define __align(x) __attribute__((aligned(x)))
DQH_T ep_QH[EP_NUM_MAX] __attribute__((aligned(2048)));
DTD_T ep_TD[EP_NUM_MAX] __attribute__((aligned(32)));
#else
DQH_T __align(2048) ep_QH[EP_NUM_MAX];
DTD_T __align(32) ep_TD[EP_NUM_MAX];
#endif

static uint32_t ep_read_len[3];

EndpointCallback USB_P_EP[USB_EP_NUM];

InterfaceEventHandler USB_Interface_Event        = NULL;
ClassRequestHandler   USB_Class_Specific_Request = NULL;
SOFHandler            USB_SOF_Event              = NULL;

uint16_t USB_DeviceStatus;
uint8_t  USB_DeviceAddress;
uint8_t  USB_Configuration;
uint32_t USB_EndPointMask;
uint32_t USB_EndPointHalt;
uint32_t USB_EndPointStall; /* EP must stay stalled */
uint8_t  USB_NumInterfaces;
uint8_t  USB_AltSetting[USB_IF_NUM];

USB_EP_DATA EP0Data;

#pragma pack(4)
uint8_t          EP0Buf[USB_MAX_PACKET0];
USB_SETUP_PACKET SetupPacket;

volatile uint32_t DevStatusFS2HS = FALSE;

static const uint8_t *USB_DeviceDescriptor;
static const uint8_t *USB_FSConfigDescriptor;
static const uint8_t *USB_HSConfigDescriptor;
static const uint8_t *USB_StringDescriptor;
static const uint8_t *USB_DeviceQualifier;
static const uint8_t *USB_FSOtherSpeedConfiguration;
static const uint8_t *USB_HSOtherSpeedConfiguration;

void USB_EndPoint0(uint32_t event);

/******************************************************************************/
/** @brief		Translates the logical endpoint address to physical
    @param[in]	EPNum	endpoint number
    @return		Physical endpoint address
*******************************************************************************/
static uint32_t EPAdr(uint32_t EPNum)
{
  uint32_t val;

  val = (EPNum & 0x0F) << 1;
  if (EPNum & 0x80)
  {
    val += 1;
  }
  return (val);
}

/******************************************************************************/
/** @brief		Set the USB device address
    @param[in]	adr		Device address
*******************************************************************************/
static void USB_SetAddress(uint32_t adr)
{
  LPC_USB->DEVICEADDR = USBDEV_ADDR(adr);
  LPC_USB->DEVICEADDR |= USBDEV_ADDR_AD;
}

/******************************************************************************/
/** @brief    Function for reseting the USB MIDI driver
*******************************************************************************/
static void USB_Reset(void)
{
  uint32_t i;

  DevStatusFS2HS = FALSE;
  /* disable all EPs */
  LPC_USB->ENDPTCTRL0 &= ~(EPCTRL_RXE | EPCTRL_TXE);
  LPC_USB->ENDPTCTRL1 &= ~(EPCTRL_RXE | EPCTRL_TXE);
  LPC_USB->ENDPTCTRL2 &= ~(EPCTRL_RXE | EPCTRL_TXE);
  LPC_USB->ENDPTCTRL3 &= ~(EPCTRL_RXE | EPCTRL_TXE);

  /* Clear all pending interrupts */
  LPC_USB->ENDPTNAK       = 0xFFFFFFFF;
  LPC_USB->ENDPTNAKEN     = 0;
  LPC_USB->USBSTS_D       = 0xFFFFFFFF;
  LPC_USB->ENDPTSETUPSTAT = LPC_USB->ENDPTSETUPSTAT;
  LPC_USB->ENDPTCOMPLETE  = LPC_USB->ENDPTCOMPLETE;
  while (LPC_USB->ENDPTPRIME) /* Wait until all bits are 0 */
  {
  }
  LPC_USB->ENDPTFLUSH = 0xFFFFFFFF;
  while (LPC_USB->ENDPTFLUSH)
    ; /* Wait until all bits are 0 */

  /* Set the interrupt Threshold control interval to 0 */
  LPC_USB->USBCMD_D &= ~0x00FF0000;

  /* Zero out the Endpoint queue heads */
  memset((void *) ep_QH, 0, EP_NUM_MAX * sizeof(DQH_T));
  /* Zero out the device transfer descriptors */
  memset((void *) ep_TD, 0, EP_NUM_MAX * sizeof(DTD_T));
  memset((void *) ep_read_len, 0, sizeof(ep_read_len));
  /* Configure the Endpoint List Address */
  /* make sure it in on 64 byte boundary !!! */
  /* init list address */
  LPC_USB->ENDPOINTLISTADDR = (uint32_t) ep_QH;
  /* Initialize device queue heads for non ISO endpoint only */
  for (i = 0; i < EP_NUM_MAX; i++)
  {
    ep_QH[i].next_dTD = (uint32_t) &ep_TD[i];
  }
  /* Enable interrupts */
  LPC_USB->USBINTR_D = USBSTS_UI
      | USBSTS_UEI
      | USBSTS_PCI
      | USBSTS_URI
      | USBSTS_SRI /* Start-of-Frame */
      | USBSTS_SLI
      | USBSTS_NAKI;
  /* enable ep0 IN and ep0 OUT */
  ep_QH[0].cap = QH_MAXP(USB_MAX_PACKET0)
      | QH_IOS
      | QH_ZLT;
  ep_QH[1].cap = QH_MAXP(USB_MAX_PACKET0)
      | QH_IOS
      | QH_ZLT;
  /* enable EP0 */
  LPC_USB->ENDPTCTRL0 = EPCTRL_RXE | EPCTRL_RXR | EPCTRL_TXE | EPCTRL_TXR;
  return;
}

/******************************************************************************/
/** @brief		Read USB setup packet from an endpoint
    @param[in]	EPNum	Endpoint number and direction
    					7	Direction (0 - out; 1-in)
    					3:0	Endpoint number
    @param[in]	pData	Pointer to data buffer
    @return		Number of bytes read
*******************************************************************************/
static uint32_t USB_ReadSetupPkt(uint32_t EPNum, uint32_t *pData)
{
  uint32_t setup_int, cnt = 0;
  uint32_t num = EPAdr(EPNum);

  setup_int = LPC_USB->ENDPTSETUPSTAT;
  /* Clear the setup interrupt */
  LPC_USB->ENDPTSETUPSTAT = setup_int;

  /*  Check if we have received a setup */
  if (setup_int & (1 << 0)) /* Check only for bit 0 */
  /* No setup are admitted on other endpoints than 0 */
  {
    do
    {
      /* Setup in a setup - must consider only the second setup */
      /*- Set the tripwire */
      LPC_USB->USBCMD_D |= USBCMD_SUTW;

      /* Transfer Set-up data to the gtmudsCore_Request buffer */
      pData[0] = ep_QH[num].setup[0];
      pData[1] = ep_QH[num].setup[1];
      cnt      = 8;

    } while (!(LPC_USB->USBCMD_D & USBCMD_SUTW));

    /* setup in a setup - Clear the tripwire */
    LPC_USB->USBCMD_D &= (~USBCMD_SUTW);
  }
  while ((setup_int = LPC_USB->ENDPTSETUPSTAT) != 0)
  {
    /* Clear the setup interrupt */
    LPC_USB->ENDPTSETUPSTAT = setup_int;
  }
  return cnt;
}

/******************************************************************************/
/** @brief		Init USB core
*******************************************************************************/
void USB_Core_Init(void)
{
  USB_P_EP[0] = USB_EndPoint0;

  /* Turn on the phy */
#if USB_PORT_FOR_MIDI == 0
  LPC_CREG->CREG0 &= ~(1 << 5);
#else
  /*                USB_AIM    USB_ESEA   USB_EPD    USB_EPWR   USB_VBUS */
  LPC_SCU->SFSUSB = (0 << 0) | (1 << 1) | (0 << 2) | (1 << 4) | (1 << 5);
#endif

  /* reset the controller */
  LPC_USB->USBCMD_D = USBCMD_RST;
  /* wait for reset to complete */
  while (LPC_USB->USBCMD_D & USBCMD_RST)
    ;

  /* Program the controller to be the USB device controller */
  LPC_USB->USBMODE_D = USBMODE_CM_DEV
      | USBMODE_SDIS
      | USBMODE_SLOM;

#if USB_PORT_FOR_MIDI == 0
  /* set OTG transcever in proper state, device is present
	on the port(CCS=1), port enable/disable status change(PES=1). */
  LPC_USB->OTGSC = (1 << 3) | (1 << 0) /*| (1<<16)| (1<<24)| (1<<25)| (1<<26)| (1<<27)| (1<<28)| (1<<29)| (1<<30)*/;
#endif

#if USB_PORT_FOR_MIDI == 0
#if USB_POLLING
  NVIC_DisableIRQ(USB0_IRQn);
#else
  NVIC_EnableIRQ(USB0_IRQn);
#endif
#else
#if USB_POLLING
  NVIC_DisableIRQ(USB1_IRQn);
#else
  NVIC_EnableIRQ(USB1_IRQn);
#endif
#endif

  USB_Reset();
  USB_SetAddress(0);

  /* USB Connect */
  LPC_USB->USBCMD_D |= USBCMD_RS;
#if !USB_POLLING
  while (!USB_Configuration)
    ;
#endif
}

/******************************************************************************/
/** @brief		Force the Full Speed for the USB controller
*******************************************************************************/
void USB_Core_ForceFullSpeed(void)
{
  LPC_USB->PORTSC1_D |= (1 << 24);
}

/******************************************************************************/
/** @brief		Set the Endpoint Callback function
    @param[in]	ep		Endpoint number
    @param[in]	cb		Callback function pointer
*******************************************************************************/
void USB_Core_Endpoint_Callback_Set(uint8_t ep, EndpointCallback cb)
{
  if ((ep >= EP_NUM_MAX) || (ep == 0))
    return;
  USB_P_EP[ep] = cb;
}

/******************************************************************************/
/** @brief		Checks whether the USB is connected and configured
    @return		1 - Success ; 0 - Failure
*******************************************************************************/
uint8_t USB_Core_IsConfigured(void)
{
  return USB_Configuration;
}

/******************************************************************************/
/** @brief		Checks if the endpoint descriptor is writable at the moment
	@param[in]	ep	Endpoint number
    @return		1 - Success ; 0 - Failure
*******************************************************************************/
uint8_t USB_Core_ReadyToWrite(uint8_t epnum)
{
  uint32_t ep = EPAdr(epnum);
  if ((ep_TD[ep].next_dTD & 1) && ((ep_TD[ep].total_bytes & 1 << 7) == 0))
    return 1;
  else
    return 0;
}

/******************************************************************************/
/** @brief		Reset USB core
*******************************************************************************/
void USB_ResetCore(void)
{

  USB_DeviceStatus  = 1;
  USB_DeviceAddress = 0;
  USB_Configuration = 0;
  USB_EndPointMask  = 0x00010001;
  USB_EndPointHalt  = 0x00000000;
  USB_EndPointStall = 0x00000000;
}

/******************************************************************************/
/** @brief		USB Request - Setup stage
*******************************************************************************/
void USB_SetupStage(void)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#pragma GCC diagnostic ignored "-Wattributes"
  USB_ReadSetupPkt(0x00, (uint32_t *) &SetupPacket);
#pragma GCC diagnostic push
}

/******************************************************************************/
/** @brief		USB Request - Data in stage
*******************************************************************************/
void USB_DataInStage(void)
{
  uint32_t cnt;

  if (EP0Data.Count > USB_MAX_PACKET0)
  {
    cnt = USB_MAX_PACKET0;
  }
  else
  {
    cnt = EP0Data.Count;
  }
  cnt = USB_WriteEP(0x80, EP0Data.pData, cnt);
  EP0Data.pData += cnt;
  EP0Data.Count -= cnt;
}

/******************************************************************************/
/** @brief		USB Request - Data out stage
*******************************************************************************/
void USB_DataOutStage(void)
{
  uint32_t cnt;

  cnt = USB_ReadEP(0x00, EP0Data.pData);
  EP0Data.pData += cnt;
  EP0Data.Count -= cnt;
}

/******************************************************************************/
/** @brief		USB Request - Status in stage
*******************************************************************************/
void USB_StatusInStage(void)
{
  USB_WriteEP(0x80, NULL, 0);
}

/******************************************************************************/
/** @brief		USB Request - Status out stage
*******************************************************************************/
void USB_StatusOutStage(void)
{
  USB_ReadEP(0x00, EP0Buf);
}

/******************************************************************************/
/** @brief		Get Status USB request
    @return		TRUE - success; FALSE - error
*******************************************************************************/
uint32_t USB_ReqGetStatus(void)
{
  uint32_t n, m;

  switch (SetupPacket.bmRequestType.BM.Recipient)
  {
    case REQUEST_TO_DEVICE:
      EP0Data.pData = (uint8_t *) &USB_DeviceStatus;
      break;
    case REQUEST_TO_INTERFACE:
      if ((USB_Configuration != 0) && (SetupPacket.wIndex.WB.L < USB_NumInterfaces))
      {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#pragma GCC diagnostic ignored "-Wattributes"
        *((__packed uint16_t *) EP0Buf) = 0;
#pragma GCC diagnostic pop
        EP0Data.pData = EP0Buf;
      }
      else
      {
        return (FALSE);
      }
      break;
    case REQUEST_TO_ENDPOINT:
      n = SetupPacket.wIndex.WB.L & 0x8F;
      m = (n & 0x80) ? ((1 << 16) << (n & 0x0F)) : (1 << n);
      if (((USB_Configuration != 0) || ((n & 0x0F) == 0)) && (USB_EndPointMask & m))
      {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
        *((__packed uint16_t *) EP0Buf) = (USB_EndPointHalt & m) ? 1 : 0;
#pragma GCC diagnostic pop
        EP0Data.pData = EP0Buf;
      }
      else
      {
        return (FALSE);
      }
      break;
    default:
      return (FALSE);
  }
  return (TRUE);
}

static void USB_WakeUpCfg(uint32_t cfg)
{
  /* Not needed */
}

static void USB_Configure(uint32_t cfg)
{
  /* Not needed */
}

static void USB_DirCtrlEP(uint32_t dir)
{
  /* Not needed */
}

/******************************************************************************/
/** @brief		USB set test mode Function
    @param[in]	mode	Test mode
    @return		TRUE if supported, else FALSE
*******************************************************************************/
static uint32_t USB_SetTestMode(uint8_t mode)
{
  uint32_t portsc;

  if ((mode > 0) && (mode < 8))
  {
    portsc = LPC_USB->PORTSC1_D & ~(0xF << 16);

    LPC_USB->PORTSC1_D = portsc | (mode << 16);
    return TRUE;
  }
  return (FALSE);
}

/******************************************************************************/
/** @brief		Set Stall for USB Endpoint
    @param[in]	EPNum	Endpoint number and direction
    					7	Direction (0 - out; 1-in)
    					3:0	Endpoint number
*******************************************************************************/
static void USB_SetStallEP(uint32_t EPNum)
{
  uint32_t lep;

  lep = EPNum & 0x0F;
  if (EPNum & 0x80)
  {
    ((uint32_t *) &(LPC_USB->ENDPTCTRL0))[lep] |= EPCTRL_TXS;
  }
  else
  {
    ((uint32_t *) &(LPC_USB->ENDPTCTRL0))[lep] |= EPCTRL_RXS;
  }
}

/******************************************************************************/
/** @brief		Clear Stall for USB Endpoint
    @param[in]	EPNum	Endpoint number and direction
    					7	Direction (0 - out; 1-in)
    					3:0	Endpoint number
*******************************************************************************/
static void USB_ClrStallEP(uint32_t EPNum)
{
  uint32_t lep;

  lep = EPNum & 0x0F;
  if (EPNum & 0x80)
  {
    ((uint32_t *) &(LPC_USB->ENDPTCTRL0))[lep] &= ~EPCTRL_TXS;
    /* reset data toggle */
    ((uint32_t *) &(LPC_USB->ENDPTCTRL0))[lep] |= EPCTRL_TXR;
  }
  else
  {
    ((uint32_t *) &(LPC_USB->ENDPTCTRL0))[lep] &= ~EPCTRL_RXS;
    /* reset data toggle */
    ((uint32_t *) &(LPC_USB->ENDPTCTRL0))[lep] |= EPCTRL_RXR;
  }
}

/******************************************************************************/
/** @brief		Set/Clear Feature USB request
    @param[in]	sc	0 - clear; 1 - set
    @return		TRUE - success; FALSE - error
*******************************************************************************/
uint32_t USB_ReqSetClrFeature(uint32_t sc)
{
  uint32_t n, m;

  switch (SetupPacket.bmRequestType.BM.Recipient)
  {
    case REQUEST_TO_DEVICE:
      if (SetupPacket.wValue.W == USB_FEATURE_REMOTE_WAKEUP)
      {
        if (sc)
        {
          USB_WakeUpCfg(TRUE);
          USB_DeviceStatus |= USB_GETSTATUS_REMOTE_WAKEUP;
        }
        else
        {
          USB_WakeUpCfg(FALSE);
          USB_DeviceStatus &= ~USB_GETSTATUS_REMOTE_WAKEUP;
        }
      }
      else if (SetupPacket.wValue.W == USB_FEATURE_TEST_MODE)
      {
        return USB_SetTestMode(SetupPacket.wIndex.WB.H);
      }
      else
      {
        return (FALSE);
      }
      break;
    case REQUEST_TO_INTERFACE:
      return (FALSE);
    case REQUEST_TO_ENDPOINT:
      n = SetupPacket.wIndex.WB.L & 0x8F;
      m = (n & 0x80) ? ((1 << 16) << (n & 0x0F)) : (1 << n);
      if ((USB_Configuration != 0) && ((n & 0x0F) != 0) && (USB_EndPointMask & m))
      {
        if (SetupPacket.wValue.W == USB_FEATURE_ENDPOINT_STALL)
        {
          if (sc)
          {
            USB_SetStallEP(n);
            USB_EndPointHalt |= m;
          }
          else
          {
            if ((USB_EndPointStall & m) != 0)
            {
              return (TRUE);
            }
            USB_ClrStallEP(n);
            USB_EndPointHalt &= ~m;
          }
        }
        else
        {
          return (FALSE);
        }
      }
      else
      {
        return (FALSE);
      }
      break;
    default:
      return (FALSE);
  }
  return (TRUE);
}

/******************************************************************************/
/** @brief		Set Address USB request
    @return		TRUE - success; FALSE - error
*******************************************************************************/
uint32_t USB_ReqSetAddress(void)
{

  switch (SetupPacket.bmRequestType.BM.Recipient)
  {
    case REQUEST_TO_DEVICE:
      USB_DeviceAddress = 0x80 | SetupPacket.wValue.WB.L;
      break;
    default:
      return (FALSE);
  }
  return (TRUE);
}

/******************************************************************************/
/** @brief		Get Descriptor USB request
    @return		TRUE - success; FALSE - error
*******************************************************************************/
uint32_t USB_ReqGetDescriptor(void)
{
  uint8_t *pD;
  uint32_t len, n;

  switch (SetupPacket.bmRequestType.BM.Recipient)
  {
    case REQUEST_TO_DEVICE:
      switch (SetupPacket.wValue.WB.H)
      {
        case USB_DEVICE_DESCRIPTOR_TYPE:
          EP0Data.pData = (uint8_t *) USB_DeviceDescriptor;
          len           = USB_DEVICE_DESC_SIZE;
          break;
        case USB_CONFIGURATION_DESCRIPTOR_TYPE:
          if (DevStatusFS2HS == FALSE)
          {
            pD = (uint8_t *) USB_FSConfigDescriptor;
          }
          else
          {
            pD = (uint8_t *) USB_HSConfigDescriptor;
          }
          for (n = 0; n != SetupPacket.wValue.WB.L; n++)
          {
            if (((USB_CONFIGURATION_DESCRIPTOR *) pD)->bLength != 0)
            {
              pD += ((USB_CONFIGURATION_DESCRIPTOR *) pD)->wTotalLength;
            }
          }
          if (((USB_CONFIGURATION_DESCRIPTOR *) pD)->bLength == 0)
          {
            return (FALSE);
          }
          EP0Data.pData = pD;
          len           = ((USB_CONFIGURATION_DESCRIPTOR *) pD)->wTotalLength;
          break;
        case USB_STRING_DESCRIPTOR_TYPE:
          pD = (uint8_t *) USB_StringDescriptor;
          for (n = 0; n != SetupPacket.wValue.WB.L; n++)
          {
            if (((USB_STRING_DESCRIPTOR *) pD)->bLength != 0)
            {
              pD += ((USB_STRING_DESCRIPTOR *) pD)->bLength;
            }
          }
          if (((USB_STRING_DESCRIPTOR *) pD)->bLength == 0)
          {
            return (FALSE);
          }
          EP0Data.pData = pD;
          len           = ((USB_STRING_DESCRIPTOR *) pD)->bLength;
          break;
        case USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE:
          /* USB Chapter 9. page 9.6.2 */
          if (DevStatusFS2HS == FALSE)
          {
            return (FALSE);
          }
          else
          {
            EP0Data.pData = (uint8_t *) USB_DeviceQualifier;
            len           = USB_DEVICE_QUALI_SIZE;
          }
          break;
        case USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE:
          if (DevStatusFS2HS == TRUE)
          {
            pD = (uint8_t *) USB_FSOtherSpeedConfiguration;
          }
          else
          {
            pD = (uint8_t *) USB_HSOtherSpeedConfiguration;
          }

          for (n = 0; n != SetupPacket.wValue.WB.L; n++)
          {
            if (((USB_OTHER_SPEED_CONFIGURATION *) pD)->bLength != 0)
            {
              pD += ((USB_OTHER_SPEED_CONFIGURATION *) pD)->wTotalLength;
            }
          }
          if (((USB_OTHER_SPEED_CONFIGURATION *) pD)->bLength == 0)
          {
            return (FALSE);
          }
          EP0Data.pData = pD;
          len           = ((USB_OTHER_SPEED_CONFIGURATION *) pD)->wTotalLength;
          break;
        default:
          return (FALSE);
      }
      break;
    case REQUEST_TO_INTERFACE:
      switch (SetupPacket.wValue.WB.H)
      {
        default:
          return (FALSE);
      }
      break;
    default:
      return (FALSE);
  }

  if (EP0Data.Count > len)
  {
    EP0Data.Count = len;
  }

  return (TRUE);
}

/******************************************************************************/
/** @brief		Configure the USB endpoint according to the descriptor
    @param[in]	pEPD	Pointer to the endpoint descriptor
*******************************************************************************/
static void USB_ConfigEP(USB_ENDPOINT_DESCRIPTOR *pEPD)
{
  uint32_t num, lep;
  uint32_t ep_cfg;
  uint8_t  bmAttributes;

  lep = pEPD->bEndpointAddress & 0x7F;
  num = EPAdr(pEPD->bEndpointAddress);

  ep_cfg = ((uint32_t *) &(LPC_USB->ENDPTCTRL0))[lep];
  /* mask the attributes we are not-interested in */
  bmAttributes = pEPD->bmAttributes & USB_ENDPOINT_TYPE_MASK;
  /* set EP type */
  if (bmAttributes != USB_ENDPOINT_TYPE_ISOCHRONOUS)
  {
    /* init EP capabilities */
    ep_QH[num].cap = QH_MAXP(pEPD->wMaxPacketSize)
        | QH_IOS | QH_ZLT;
    /* The next DTD pointer is INVALID */
    ep_TD[num].next_dTD = 0x01;
  }
  else
  {
    /* init EP capabilities */
    ep_QH[num].cap = QH_MAXP(pEPD->wMaxPacketSize) | QH_ZLT | (1 << 30);
    /* The next DTD pointer is INVALID */
    ep_QH[num].next_dTD = ep_TD[num].next_dTD = 0x01;
  }
  /* setup EP control register */
  if (pEPD->bEndpointAddress & 0x80)
  {
    ep_cfg &= ~0xFFFF0000;
    ep_cfg |= EPCTRL_TX_TYPE(bmAttributes)
        | EPCTRL_TXR;
  }
  else
  {
    ep_cfg &= ~0xFFFF;
    ep_cfg |= EPCTRL_RX_TYPE(bmAttributes)
        | EPCTRL_RXR;
  }
  ((uint32_t *) &(LPC_USB->ENDPTCTRL0))[lep] = ep_cfg;
  return;
}

/******************************************************************************/
/** @brief		Enable the USB endpoint
    @param[in]	EPNum	Endpoint number and direction
    					7	Direction (0 - out; 1-in)
    					3:0	Endpoint number
*******************************************************************************/
static void USB_EnableEP(uint32_t EPNum)
{
  uint32_t lep, bitpos;

  lep = EPNum & 0x0F;

  if (EPNum & 0x80)
  {
    ((uint32_t *) &(LPC_USB->ENDPTCTRL0))[lep] |= EPCTRL_TXE;
#if USB_POLLING
    /* enable NAK interrupt for IN transfers - useful in polling mode only*/
    bitpos = USB_EP_BITPOS(EPNum);
    LPC_USB->ENDPTNAKEN |= (1 << bitpos);
#endif
  }
  else
  {
    ((uint32_t *) &(LPC_USB->ENDPTCTRL0))[lep] |= EPCTRL_RXE;
    /* enable NAK interrupt */
    bitpos = USB_EP_BITPOS(EPNum);
    LPC_USB->ENDPTNAKEN |= (1 << bitpos);
  }
}

/******************************************************************************/
/** @brief		Disable the USB endpoint
    @param[in]	EPNum	Endpoint number and direction
    					7	Direction (0 - out; 1-in)
    					3:0	Endpoint number
*******************************************************************************/
static void USB_DisableEP(uint32_t EPNum)
{
  uint32_t lep, bitpos;

  lep = EPNum & 0x0F;
  if (EPNum & 0x80)
  {
#if USB_POLLING
    /* disable NAK interrupt */
    bitpos = USB_EP_BITPOS(EPNum);
    LPC_USB->ENDPTNAKEN |= ~(1 << bitpos);
#endif
    ((uint32_t *) &(LPC_USB->ENDPTCTRL0))[lep] &= ~EPCTRL_TXE;
  }
  else
  {
    /* disable NAK interrupt */
    bitpos = USB_EP_BITPOS(EPNum);
    LPC_USB->ENDPTNAKEN &= ~(1 << bitpos);
    ((uint32_t *) &(LPC_USB->ENDPTCTRL0))[lep] &= ~EPCTRL_RXE;
  }
}

/******************************************************************************/
/** @brief		Reset the USB endpoint
    @param[in]	EPNum	Endpoint number and direction
    					7	Direction (0 - out; 1-in)
    					3:0	Endpoint number
*******************************************************************************/
void USB_ResetEP(uint32_t EPNum)
{
  uint32_t bit_pos = USB_EP_BITPOS(EPNum);
  uint32_t lep     = EPNum & 0x0F;

  /* flush EP buffers */
  LPC_USB->ENDPTFLUSH = (1 << bit_pos);
  while (LPC_USB->ENDPTFLUSH & (1 << bit_pos))
    ;
  /* reset data toggles */
  if (EPNum & 0x80)
  {
    ((uint32_t *) &(LPC_USB->ENDPTCTRL0))[lep] |= EPCTRL_TXR;
  }
  else
  {
    ((uint32_t *) &(LPC_USB->ENDPTCTRL0))[lep] |= EPCTRL_RXR;
  }
}

/******************************************************************************/
/** @brief		Get Configuration USB request
    @return		TRUE - success; FALSE - error
*******************************************************************************/
uint32_t USB_ReqGetConfiguration(void)
{

  switch (SetupPacket.bmRequestType.BM.Recipient)
  {
    case REQUEST_TO_DEVICE:
      EP0Data.pData = &USB_Configuration;
      break;
    default:
      return (FALSE);
  }
  return (TRUE);
}

/******************************************************************************/
/** @brief		Set Configuration USB request
    @return		TRUE - success; FALSE - error
*******************************************************************************/
uint32_t USB_ReqSetConfiguration(void)
{
  USB_COMMON_DESCRIPTOR *pD;
  uint32_t               alt = 0;
  uint32_t               n, m;
  uint32_t               new_addr;
  switch (SetupPacket.bmRequestType.BM.Recipient)
  {
    case REQUEST_TO_DEVICE:

      if (SetupPacket.wValue.WB.L)
      {
        if (DevStatusFS2HS == FALSE)
        {
          pD = (USB_COMMON_DESCRIPTOR *) USB_FSConfigDescriptor;
        }
        else
        {
          pD = (USB_COMMON_DESCRIPTOR *) USB_HSConfigDescriptor;
        }
        while (pD->bLength)
        {
          switch (pD->bDescriptorType)
          {
            case USB_CONFIGURATION_DESCRIPTOR_TYPE:
              if (((USB_CONFIGURATION_DESCRIPTOR *) pD)->bConfigurationValue == SetupPacket.wValue.WB.L)
              {
                USB_Configuration = SetupPacket.wValue.WB.L;
                USB_NumInterfaces = ((USB_CONFIGURATION_DESCRIPTOR *) pD)->bNumInterfaces;
                for (n = 0; n < USB_IF_NUM; n++)
                {
                  USB_AltSetting[n] = 0;
                }
                for (n = 1; n < USB_EP_NUM; n++)
                {
                  if (USB_EndPointMask & (1 << n))
                  {
                    USB_DisableEP(n);
                  }
                  if (USB_EndPointMask & ((1 << 16) << n))
                  {
                    USB_DisableEP(n | 0x80);
                  }
                }
                USB_EndPointMask  = 0x00010001;
                USB_EndPointHalt  = 0x00000000;
                USB_EndPointStall = 0x00000000;
                USB_Configure(TRUE);
                if (((USB_CONFIGURATION_DESCRIPTOR *) pD)->bmAttributes & USB_CONFIG_POWERED_MASK)
                {
                  USB_DeviceStatus |= USB_GETSTATUS_SELF_POWERED;
                }
                else
                {
                  USB_DeviceStatus &= ~USB_GETSTATUS_SELF_POWERED;
                }
              }
              else
              {
                new_addr = (uint32_t) pD + ((USB_CONFIGURATION_DESCRIPTOR *) pD)->wTotalLength;
                pD       = (USB_COMMON_DESCRIPTOR *) new_addr;
                continue;
              }
              break;
            case USB_INTERFACE_DESCRIPTOR_TYPE:
              alt = ((USB_INTERFACE_DESCRIPTOR *) pD)->bAlternateSetting;
              break;
            case USB_ENDPOINT_DESCRIPTOR_TYPE:
              if (alt == 0)
              {
                n = ((USB_ENDPOINT_DESCRIPTOR *) pD)->bEndpointAddress & 0x8F;
                m = (n & 0x80) ? ((1 << 16) << (n & 0x0F)) : (1 << n);
                USB_EndPointMask |= m;
                USB_ConfigEP((USB_ENDPOINT_DESCRIPTOR *) pD);
                USB_EnableEP(n);
                USB_ResetEP(n);
              }
              break;
          }
          new_addr = (uint32_t) pD + pD->bLength;
          pD       = (USB_COMMON_DESCRIPTOR *) new_addr;
        }
      }
      else
      {
        USB_Configuration = 0;
        for (n = 1; n < USB_EP_NUM; n++)
        {
          if (USB_EndPointMask & (1 << n))
          {
            USB_DisableEP(n);
          }
          if (USB_EndPointMask & ((1 << 16) << n))
          {
            USB_DisableEP(n | 0x80);
          }
        }
        USB_EndPointMask  = 0x00010001;
        USB_EndPointHalt  = 0x00000000;
        USB_EndPointStall = 0x00000000;
        USB_Configure(FALSE);
      }

      if (USB_Configuration != SetupPacket.wValue.WB.L)
      {
        return (FALSE);
      }
      break;
    default:
      return (FALSE);
  }
  return (TRUE);
}

/******************************************************************************/
/** @brief		Get Interface USB request
    @return		TRUE - success; FALSE - error
*******************************************************************************/
uint32_t USB_ReqGetInterface(void)
{

  switch (SetupPacket.bmRequestType.BM.Recipient)
  {
    case REQUEST_TO_INTERFACE:
      if ((USB_Configuration != 0) && (SetupPacket.wIndex.WB.L < USB_NumInterfaces))
      {
        EP0Data.pData = USB_AltSetting + SetupPacket.wIndex.WB.L;
      }
      else
      {
        return (FALSE);
      }
      break;
    default:
      return (FALSE);
  }
  return (TRUE);
}

/******************************************************************************/
/** @brief		Set Interface USB request
    @return		TRUE - success; FALSE - error
*******************************************************************************/
uint32_t USB_ReqSetInterface(void)
{
  USB_COMMON_DESCRIPTOR *pD;
  uint32_t               ifn = 0, alt = 0, old = 0, msk = 0;
  uint32_t               n, m;
  uint32_t               set, new_addr;

  switch (SetupPacket.bmRequestType.BM.Recipient)
  {
    case REQUEST_TO_INTERFACE:
      if (USB_Configuration == 0)
        return (FALSE);
      set = FALSE;
      if (DevStatusFS2HS == FALSE)
      {
        pD = (USB_COMMON_DESCRIPTOR *) USB_FSConfigDescriptor;
      }
      else
      {
        pD = (USB_COMMON_DESCRIPTOR *) USB_HSConfigDescriptor;
      }
      while (pD->bLength)
      {
        switch (pD->bDescriptorType)
        {
          case USB_CONFIGURATION_DESCRIPTOR_TYPE:
            if (((USB_CONFIGURATION_DESCRIPTOR *) pD)->bConfigurationValue != USB_Configuration)
            {
              new_addr = (uint32_t) pD + ((USB_CONFIGURATION_DESCRIPTOR *) pD)->wTotalLength;
              pD       = (USB_COMMON_DESCRIPTOR *) new_addr;
              continue;
            }
            break;
          case USB_INTERFACE_DESCRIPTOR_TYPE:
            ifn = ((USB_INTERFACE_DESCRIPTOR *) pD)->bInterfaceNumber;
            alt = ((USB_INTERFACE_DESCRIPTOR *) pD)->bAlternateSetting;
            msk = 0;
            if ((ifn == SetupPacket.wIndex.WB.L) && (alt == SetupPacket.wValue.WB.L))
            {
              set                 = TRUE;
              old                 = USB_AltSetting[ifn];
              USB_AltSetting[ifn] = (uint8_t) alt;
            }
            break;
          case USB_ENDPOINT_DESCRIPTOR_TYPE:
            if (ifn == SetupPacket.wIndex.WB.L)
            {
              n = ((USB_ENDPOINT_DESCRIPTOR *) pD)->bEndpointAddress & 0x8F;
              m = (n & 0x80) ? ((1 << 16) << (n & 0x0F)) : (1 << n);
              if (alt == SetupPacket.wValue.WB.L)
              {
                USB_EndPointMask |= m;
                USB_EndPointHalt &= ~m;
                USB_ConfigEP((USB_ENDPOINT_DESCRIPTOR *) pD);
                USB_EnableEP(n);
                USB_ResetEP(n);
                msk |= m;
              }
              else if ((alt == old) && ((msk & m) == 0))
              {
                USB_EndPointMask &= ~m;
                USB_EndPointHalt &= ~m;
                USB_DisableEP(n);
              }
            }
            break;
        }
        new_addr = (uint32_t) pD + pD->bLength;
        pD       = (USB_COMMON_DESCRIPTOR *) new_addr;
      }
      break;
    default:
      return (FALSE);
  }

  return (set);
}

/******************************************************************************/
/** @brief		Endpoint 0 Callback
    @param[in]	event	Event that triggered the interrupt
*******************************************************************************/
void USB_EndPoint0(uint32_t event)
{
  switch (event)
  {
    case USB_EVT_SETUP:
      USB_SetupStage();
      USB_DirCtrlEP(SetupPacket.bmRequestType.BM.Dir);
      EP0Data.Count = SetupPacket.wLength; /* Number of bytes to transfer */
      switch (SetupPacket.bmRequestType.BM.Type)
      {

        case REQUEST_STANDARD:
          switch (SetupPacket.bRequest)
          {
            case USB_REQUEST_GET_STATUS:
              if (!USB_ReqGetStatus())
              {
                goto stall_i;
              }
              USB_DataInStage();
              break;

            case USB_REQUEST_CLEAR_FEATURE:
              if (!USB_ReqSetClrFeature(0))
              {
                goto stall_i;
              }
              USB_StatusInStage();
#if USB_FEATURE_EVENT
              USB_Feature_Event();
#endif
              break;

            case USB_REQUEST_SET_FEATURE:
              if (!USB_ReqSetClrFeature(1))
              {
                goto stall_i;
              }
              USB_StatusInStage();
#if USB_FEATURE_EVENT
              USB_Feature_Event();
#endif
              break;

            case USB_REQUEST_SET_ADDRESS:
              if (!USB_ReqSetAddress())
              {
                goto stall_i;
              }
              USB_StatusInStage();
              break;

            case USB_REQUEST_GET_DESCRIPTOR:
              if (!USB_ReqGetDescriptor())
              {
                goto stall_i;
              }
              USB_DataInStage();
              break;

            case USB_REQUEST_SET_DESCRIPTOR:
              /*stall_o:*/ USB_SetStallEP(0x00); /* not supported */
              EP0Data.Count = 0;
              break;

            case USB_REQUEST_GET_CONFIGURATION:
              if (!USB_ReqGetConfiguration())
              {
                goto stall_i;
              }
              USB_DataInStage();
              break;

            case USB_REQUEST_SET_CONFIGURATION:
              if (!USB_ReqSetConfiguration())
              {
                goto stall_i;
              }
              USB_StatusInStage();
#if USB_CONFIGURE_EVENT
              USB_Configure_Event();
#endif
              break;

            case USB_REQUEST_GET_INTERFACE:
              if (!USB_ReqGetInterface())
              {
                goto stall_i;
              }
              USB_DataInStage();
              break;

            case USB_REQUEST_SET_INTERFACE:
              if (!USB_ReqSetInterface())
              {
                goto stall_i;
              }
              USB_StatusInStage();
#if 1
              if (USB_Interface_Event)
                USB_Interface_Event(&SetupPacket);
#endif
              break;

            default:
              goto stall_i;
          }
          break; /* end case REQUEST_STANDARD */

        case REQUEST_CLASS:
          if (USB_Class_Specific_Request)
          {
            if (USB_Class_Specific_Request(&SetupPacket, &EP0Data, event))
              goto stall_i;
          }
          break; /* end case REQUEST_CLASS */
        default:
        stall_i:
          USB_SetStallEP(0x80);
          EP0Data.Count = 0;
          break;
      }
      break; /* end case USB_EVT_SETUP */

    case USB_EVT_OUT_NAK:
      if (SetupPacket.bmRequestType.BM.Dir == 0)
      {
        USB_ReadReqEP(0x00, EP0Data.pData, EP0Data.Count);
      }
      else
      {
        /* might be zero length pkt */
        USB_ReadReqEP(0x00, EP0Data.pData, 0);
      }
      break;
    case USB_EVT_OUT:
      if (SetupPacket.bmRequestType.BM.Dir == REQUEST_HOST_TO_DEVICE)
      {
        if (EP0Data.Count)
        {                     /* still data to receive ? */
          USB_DataOutStage(); /* receive data */
          if (EP0Data.Count == 0)
          { /* data complete ? */
            switch (SetupPacket.bmRequestType.BM.Type)
            {

              case REQUEST_STANDARD:
                goto stall_i;
                break; /* not supported */
              case REQUEST_CLASS:
                if (USB_Class_Specific_Request)
                {
                  if (USB_Class_Specific_Request(&SetupPacket, &EP0Data, event))
                    goto stall_i;
                }
                break;
              default:
                goto stall_i;
            }
          }
        }
      }
      else
      {
        USB_StatusOutStage(); /* receive Acknowledge */
      }
      break; /* end case USB_EVT_OUT */

    case USB_EVT_IN:
      if (SetupPacket.bmRequestType.BM.Dir == REQUEST_DEVICE_TO_HOST)
      {
        USB_DataInStage(); /* send data */
      }
      else
      {
        if (USB_DeviceAddress & 0x80)
        {
          USB_DeviceAddress &= 0x7F;
          USB_SetAddress(USB_DeviceAddress);
        }
      }
      break; /* end case USB_EVT_IN */

    case USB_EVT_OUT_STALL:
      USB_ClrStallEP(0x00);
      break;

    case USB_EVT_IN_STALL:
      USB_ClrStallEP(0x80);
      break;
  }
}

/******************************************************************************/
/** @brief		USB Interrupt Service Routine
*******************************************************************************/
#if USB_PORT_FOR_MIDI == 0
void USB0_IRQHandler(void)
#else
void USB1_IRQHandler(void)
#endif
{
  uint32_t disr, val, n;

  disr              = LPC_USB->USBSTS_D; /* Device Interrupt Status */
  LPC_USB->USBSTS_D = disr;

  /* Device Status Interrupt (Reset, Connect change, Suspend/Resume) */
  if (disr & USBSTS_URI) /* Reset */
  {
    USB_Reset();
    USB_ResetCore();
    return;
  }

  if (disr & USBSTS_SLI) /* Suspend */
  {
  }

  if (disr & USBSTS_PCI) /* Resume */
  {
    /* check if device is operating in HS mode or full speed */
    if (LPC_USB->PORTSC1_D & (1 << 9))
      DevStatusFS2HS = TRUE;
  }

  /* handle setup status interrupts */
  val = LPC_USB->ENDPTSETUPSTAT;
  /* Only EP0 will have setup packets so call EP0 handler */
  if (val)
  {
    /* Clear the endpoint complete CTRL OUT & IN when */
    /* a Setup is received */
    LPC_USB->ENDPTCOMPLETE = 0x00010001;
    /* enable NAK inetrrupts */
    LPC_USB->ENDPTNAKEN |= 0x00010001;

    USB_P_EP[0](USB_EVT_SETUP);
  }

  /* handle completion interrupts */
  val = LPC_USB->ENDPTCOMPLETE;
  if (val)
  {
    LPC_USB->ENDPTNAK = val;

    /* EP 0 - OUT */
    if (val & 1)
    {
      LPC_USB->ENDPTCOMPLETE = 1;
      USB_P_EP[0](USB_EVT_OUT);
    }
    /* EP 0 - IN */
    if (val & (1 << 16))
    {
      ep_TD[1].total_bytes &= 0xC0;
      LPC_USB->ENDPTCOMPLETE = (1 << 16);
      USB_P_EP[0](USB_EVT_IN);
    }
    /* EP 1 - OUT */
    if (val & 2)
    {
      LPC_USB->ENDPTCOMPLETE = 2;
      USB_P_EP[1](USB_EVT_OUT);
    }
    /* EP 1 - IN */
    if (val & (1 << 17))
    {
      ep_TD[3].total_bytes &= 0xC0;
      LPC_USB->ENDPTCOMPLETE = (1 << 17);
      USB_P_EP[1](USB_EVT_IN);
    }
    /* EP 2 - IN */
    if (val & (1 << 18))
    {
      ep_TD[5].total_bytes &= 0xC0;
      LPC_USB->ENDPTCOMPLETE = (1 << 18);
      USB_P_EP[2](USB_EVT_IN);
    }
  }

  if (disr & USBSTS_NAKI)
  {
    val = LPC_USB->ENDPTNAK;
    val &= LPC_USB->ENDPTNAKEN;
    /* handle NAK interrupts */
    if (val)
    {
      LPC_USB->ENDPTNAK = val;
      for (n = 0; n < EP_NUM_MAX / 2; n++)
      {
        if (val & (1 << n))
          USB_P_EP[n](USB_EVT_OUT_NAK);
        if (val & (1 << (n + 16)))
          USB_P_EP[n](USB_EVT_IN_NAK);
      }
    }
  }

  /* Start of Frame Interrupt */
  if (disr & USBSTS_SRI)
  {
    if (USB_SOF_Event)
      USB_SOF_Event();
  }

  /* Error Interrupt */
  if (disr & USBSTS_UEI)
  {
  }

  return;
}

/******************************************************************************/
/** @brief		Program the dTD descriptor
    @param[in]	Edpt	Endpoint number in the endpoint queue
    @param[in]	ptrBuff	Pointer to data buffer
    @param[in]	TsfSize	Size of the transfer buffer
*******************************************************************************/
void USB_ProgDTD(uint32_t Edpt, uint32_t ptrBuff, uint32_t TsfSize)
{
  DTD_T *pDTD;

  pDTD = (DTD_T *) &ep_TD[Edpt];

  /* Zero out the device transfer descriptors */
  memset((void *) pDTD, 0, sizeof(DTD_T));
  /* The next DTD pointer is INVALID */
  pDTD->next_dTD = 0x01;

  /* Length */
  pDTD->total_bytes = ((TsfSize & 0x7fff) << 16);
  pDTD->total_bytes |= TD_IOC;
  pDTD->total_bytes |= 0x80;

  pDTD->buffer0 = ptrBuff;
  pDTD->buffer1 = (ptrBuff + 0x1000) & 0xfffff000;
  pDTD->buffer2 = (ptrBuff + 0x2000) & 0xfffff000;
  pDTD->buffer3 = (ptrBuff + 0x3000) & 0xfffff000;
  pDTD->buffer4 = (ptrBuff + 0x4000) & 0xfffff000;

  ep_QH[Edpt].next_dTD = (uint32_t)(&ep_TD[Edpt]);
  ep_QH[Edpt].total_bytes &= (~0xC0);
}

/******************************************************************************/
/** @brief		Write USB endpoint data
    @param[in]	EPNum	Endpoint number and direction
    					7	Direction (0 - out; 1-in)
    					3:0	Endpoint number
    @param[in]	pData	Pointer to data buffer
    @param[in]	cnt		Number of bytes to write
    @return		Number of bytes written
*******************************************************************************/
uint32_t USB_WriteEP(uint32_t EPNum, uint8_t *pData, uint32_t cnt)
{
  uint32_t n = USB_EP_BITPOS(EPNum);

  USB_ProgDTD(EPAdr(EPNum), (uint32_t) pData, cnt);
  /* prime the endpoint for transmit */
  LPC_USB->ENDPTPRIME |= (1 << n);

  return (cnt);
}

/******************************************************************************/
/** @brief		Read USB endpoint data
    @param[in]	EPNum	Endpoint number and direction
    					7	Direction (0 - out; 1-in)
    					3:0	Endpoint number
    @param[in]	pData	Pointer to data buffer
    @return		Number of bytes read
*******************************************************************************/
uint32_t USB_ReadEP(uint32_t EPNum, uint8_t *pData)
{
  uint32_t cnt, n;
  DTD_T *  pDTD;

  n    = EPAdr(EPNum);
  pDTD = (DTD_T *) &ep_TD[n];

  /* return the total bytes read */
  cnt = (pDTD->total_bytes >> 16) & 0x7FFF;
  cnt = ep_read_len[EPNum & 0x0F] - cnt;
  return (cnt);
}

/******************************************************************************/
/** @brief		Enqueue read request
    @param[in]	EPNum	Endpoint number and direction
    					7	Direction (0 - out; 1-in)
    					3:0	Endpoint number
    @param[in]	pData	Pointer to data buffer
    @param[in]	len		Length of the data buffer
    @return		Length of the data buffer
*******************************************************************************/
uint32_t USB_ReadReqEP(uint32_t EPNum, uint8_t *pData, uint32_t len)
{
  uint32_t num = EPAdr(EPNum);
  uint32_t n   = USB_EP_BITPOS(EPNum);

  USB_ProgDTD(num, (uint32_t) pData, len);
  ep_read_len[EPNum & 0x0F] = len;
  /* prime the endpoint for read */
  LPC_USB->ENDPTPRIME |= (1 << n);
  return len;
}

/******************************************************************************/
/** @brief		Gets the number of bytes left to be sent
    @param[in]	endbuff	End of buffer address
    @param[in]	ep		Endpoint number and direction
    					7	Direction (0 - out; 1-in)
    					3:0	Endpoint number
    @return		Number of bytes left to be sent
*******************************************************************************/
uint32_t USB_Core_BytesToSend(uint32_t endbuff, uint32_t ep)
{
  uint32_t x = ep_QH[EPAdr(ep)].buffer0;
  if (x == 0)  // seems to fix the "USB reconnect traffic jam"
    return 0;
  return ((endbuff - x) & 0xFFF);
}

void USB_Core_Device_Descriptor_Set(const uint8_t *ddesc)
{
  USB_DeviceDescriptor = ddesc;
}

void USB_Core_Device_FS_Descriptor_Set(const uint8_t *fsdesc)
{
  USB_FSConfigDescriptor        = fsdesc;
  USB_FSOtherSpeedConfiguration = fsdesc;
}

void USB_Core_Device_HS_Descriptor_Set(const uint8_t *hsdesc)
{
  USB_HSConfigDescriptor        = hsdesc;
  USB_HSOtherSpeedConfiguration = hsdesc;
}

void USB_Core_Device_String_Descriptor_Set(const uint8_t *strdesc)
{
  USB_StringDescriptor = strdesc;
}

void USB_Core_Device_Device_Quali_Descriptor_Set(const uint8_t *dqdesc)
{
  USB_DeviceQualifier = dqdesc;
}

void USB_Core_Interface_Event_Handler_Set(InterfaceEventHandler ievh)
{
  USB_Interface_Event = ievh;
}

void USB_Core_Class_Request_Handler_Set(ClassRequestHandler csrqh)
{
  USB_Class_Specific_Request = csrqh;
}

void USB_Core_SOF_Event_Handler_Set(SOFHandler sofh)
{
  USB_SOF_Event = sofh;
}
