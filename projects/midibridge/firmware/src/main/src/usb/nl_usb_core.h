/******************************************************************************/
/** @file		nl_usb_core.h
    @date		2014-12-11
    @brief    	Core functionality for the USB
    @example
    @ingroup	nl_drv_modules
    @author		Nemanja Nikodijevic 2014-12-11
*******************************************************************************/
#pragma once

#include "usb/nl_usbd.h"
#include "globals.h"

/** USB controller used by this driver */
#if USB_PORT_FOR_MIDI == 0
#define LPC_USB LPC_USB0
void USB0_IRQHandler(void);
#else
#define LPC_USB LPC_USB1
void USB1_IRQHandler(void);
#endif

/** Total number of interfaces*/
#define USB_IF_NUM 3
/** Total physical endpoints*/
#define EP_NUM_MAX 6
/** Total logical endpoints */
#define USB_EP_NUM 3
/** Maximum length of a Control endpoint packet */
#define USB_MAX_PACKET0 64

/** USB driver in polling mode? */
#define USB_POLLING 1

/* USB Endpoint Data Structure */
typedef struct _USB_EP_DATA
{
  uint8_t* pData;
  uint16_t Count;
} USB_EP_DATA;

/* Definition for Endpoint Callback function */
typedef void (*EndpointCallback)(uint32_t event);
/* Definition for the Interface Event handler function */
typedef void (*InterfaceEventHandler)(USB_SETUP_PACKET* spkt);
/* Definition for the Class-specific request handler function */
typedef uint8_t (*ClassRequestHandler)(USB_SETUP_PACKET* spkt, USB_EP_DATA* ep0dat, uint32_t event);
/* Definition for the Start of Frame handler */
typedef void (*SOFHandler)(void);

/** dTD Transfer Descriptor */
typedef volatile struct
{
  volatile uint32_t next_dTD;    /**< Next Transfer Descriptor in the queue */
  volatile uint32_t total_bytes; /**< dTD Token field:
  	  	  	  	  	  	  	  	  	   30:16 Total number of bytes to transfer \n
   	   	   	   	   	   	   	   	   	   15	 IOC - Interrupt on Complete \n
   	   	   	   	   	   	   	   	   	   11:10 Multiplier Override (only for ISO) \n
   	   	   	   	   	   	   	   	   	   7:0	 Status field \n
   	   	   	   	   	   	   	   	   	   	   Bit 7 = 1 - status: Active \n
										   Bit 6 = 1 - status: Halted \n
									   	   Bit 5 = 1 - status: Buffer Error \n
										   Bit 4 - reserved \n
										   Bit 3 = 1 - status: Transaction Error \n
										   Bit 2 - reserved \n
										   Bit 1 - reserved \n
										   Bit 0 - reserved */
  volatile uint32_t buffer0;     /**< Pointer to the page 0 of the transfer buffer */
  volatile uint32_t buffer1;     /**< Pointer to the page 1 of the transfer buffer */
  volatile uint32_t buffer2;     /**< Pointer to the page 2 of the transfer buffer */
  volatile uint32_t buffer3;     /**< Pointer to the page 3 of the transfer buffer */
  volatile uint32_t buffer4;     /**< Pointer to the page 4 of the transfer buffer */
  volatile uint32_t reserved;    /**< Reserved field */
} DTD_T;

/** dQH  Queue Head */
typedef volatile struct
{
  volatile uint32_t cap;         /**< Endpoint Capabilities field: \n
  	  	  	  	  	  	  	  31:30	MULT (only for ISO) \n
  	  	  	  	  	  	  	  29	Zero Length Termination (0-Enable; 1-Disable) \n
  	  	  	  	  	  	  	  26:16 Maximum Packet Length (<=1024) \n
  	  	  	  	  	  	  	  15	Interrupt on Setup */
  volatile uint32_t curr_dTD;    /**< Current dTD pointer */
  volatile uint32_t next_dTD;    /**< Next Transfer Descriptor in the queue */
  volatile uint32_t total_bytes; /**< Current dTD Overlay - total_bytes field */
  volatile uint32_t buffer0;     /**< Current dTD Overlay - buffer page 0 */
  volatile uint32_t buffer1;     /**< Current dTD Overlay - buffer page 1 */
  volatile uint32_t buffer2;     /**< Current dTD Overlay - buffer page 2 */
  volatile uint32_t buffer3;     /**< Current dTD Overlay - buffer page 3 */
  volatile uint32_t buffer4;     /**< Current dTD Overlay - buffer page 4 */
  volatile uint32_t reserved;    /**< Current dTD Overlay - reserved field */
  volatile uint32_t setup[2];    /**< Setup buffers */
  volatile uint32_t gap[4];      /**< Gap to fit to 64-byte boundary */
} DQH_T;

/** bit defines for USBCMD register
 * @{
 */
/** Run/Stop */
#define USBCMD_RS (1 << 0)
/** Controller Reset */
#define USBCMD_RST (1 << 1)
/** Setup trip wire */
#define USBCMD_SUTW (1 << 13)
/** Add dTD trip wire */
#define USBCMD_ATDTW (1 << 14)
/** @} */

/** bit defines for USBSTS register
 * @{
 */
/** USB Interrupt */
#define USBSTS_UI (1 << 0)
/** USB Error Interrupt */
#define USBSTS_UEI (1 << 1)
/** Port change detect */
#define USBSTS_PCI (1 << 2)
/** USB Reset received */
#define USBSTS_URI (1 << 6)
/** Start-of-Frame received */
#define USBSTS_SRI (1 << 7)
/** Device Controller Suspended */
#define USBSTS_SLI (1 << 8)
/** NAK Interrupt */
#define USBSTS_NAKI (1 << 16)
/** @} */

/** bit defines for DEVICEADDR register
 * @{
 */
/** Device address advance */
#define USBDEV_ADDR_AD (1 << 24)
/** USB device address */
#define USBDEV_ADDR(n) (((n) &0x7F) << 25)
/** @} */

/** bit defines for USBMODE register
 * @{
 */
/** Controller mode: IDLE */
#define USBMODE_CM_IDLE (0x0 << 0)
/** Controller mode: DEVICE */
#define USBMODE_CM_DEV (0x2 << 0)
/** Controller mode: HOST */
#define USBMODE_CM_HOST (0x3 << 0)
/** Setup Lockout mode */
#define USBMODE_SLOM (1 << 3)
/** Stream Disable mode */
#define USBMODE_SDIS (1 << 4)
/** @} */

/** bit defines for EP registers
 * @{
 */
/** 5:0 Receive Endpoints 5 to 0 \n
 *  21:16 Transmit Endpoints 5:0
 */
#define USB_EP_BITPOS(n) (((n) &0x80) ? (0x10 | ((n) &0x7)) : ((n) &0x7))
/** @} */

/** bit defines EPcontrol registers
 * @{
 */
/** Rx endpoint stall */
#define EPCTRL_RXS (1 << 0)
/** Rx endpoint type: \n
 * 	0x0 - Control \n
 * 	0x1 - Isochronous \n
 * 	0x2 - Bulk \n
 * 	0x3 - Interrupt */
#define EPCTRL_RX_TYPE(n) (((n) &0x3) << 2)
#define EPCTRL_RX_CTL     (0 << 2)
#define EPCTRL_RX_ISO     (1 << 2)
#define EPCTRL_RX_BLK     (2 << 2)
/** Rx data toggle inhibit */
#define EPCTRL_RXI (1 << 5)
/** Rx data toggle reset */
#define EPCTRL_RXR (1 << 6)
/** Rx endpoint enable */
#define EPCTRL_RXE (1 << 7)
/** Tx endpoint stall */
#define EPCTRL_TXS (1 << 16)
/** Tx endpoint type: \n
 * 	0x0 - Control \n
 * 	0x1 - Isochronous \n
 * 	0x2 - Bulk \n
 * 	0x3 - Interrupt */
#define EPCTRL_TX_TYPE(n) (((n) &0x3) << 18)
#define EPCTRL_TX_CTL     (0 << 18)
#define EPCTRL_TX_ISO     (1 << 18)
#define EPCTRL_TX_BLK     (2 << 18)
#define EPCTRL_TX_INT     (3 << 18)
/** Tx data toggle inhibit */
#define EPCTRL_TXI (1 << 21)
/** Tx data toggle reset */
#define EPCTRL_TXR (1 << 22)
/** Tx endpoint enable */
#define EPCTRL_TXE (1 << 23)
/** @} */

/** dQH field and bit defines
 * @{
 */
/** Maximum packet length */
#define QH_MAXP(n) (((n) &0x3FF) << 16)
/** Interrupt on setup */
#define QH_IOS (1 << 15)
/** Zero length termination */
#define QH_ZLT (1 << 29)
/** @} */

/** dTD field and bit defines
 * @{
 */
/** Terminate the next dTD pointer; end of dTD queue */
#define TD_NEXT_TERMINATE (1 << 0)
/** Interrupt on complete */
#define TD_IOC (1 << 15)
/** @} */

/* USB Core Functions */
void     USB_Core_Init(void);
void     USB_Core_Endpoint_Callback_Set(uint8_t ep, EndpointCallback cb);
uint8_t  USB_Core_IsConfigured(void);
uint8_t  USB_Core_ReadyToWrite(uint8_t epnum);
uint32_t USB_Core_BytesToSend(uint32_t endbuff, uint32_t ep);
void     USB_Core_ForceFullSpeed(void);

void USB_Core_Interface_Event_Handler_Set(InterfaceEventHandler ievh);
void USB_Core_Class_Request_Handler_Set(ClassRequestHandler csrqh);
void USB_Core_SOF_Event_Handler_Set(SOFHandler sofh);

/* Assigning descriptors to the driver */
void USB_Core_Device_Descriptor_Set(const uint8_t* ddesc);
void USB_Core_Device_FS_Descriptor_Set(const uint8_t* fsdesc);
void USB_Core_Device_HS_Descriptor_Set(const uint8_t* hsdesc);
void USB_Core_Device_String_Descriptor_Set(const uint8_t* strdesc);
void USB_Core_Device_Device_Quali_Descriptor_Set(const uint8_t* dqdesc);

uint32_t USB_WriteEP(uint32_t EPNum, uint8_t* pData, uint32_t cnt);
uint32_t USB_ReadEP(uint32_t EPNum, uint8_t* pData);
uint32_t USB_ReadReqEP(uint32_t EPNum, uint8_t* pData, uint32_t len);
void     USB_ResetEP(uint32_t EPNum);

void USB_DataInStage(void);
void USB_DataOutStage(void);
void USB_StatusInStage(void);
void USB_StatusOutStage(void);
