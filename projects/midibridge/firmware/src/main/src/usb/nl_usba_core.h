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
#include "usb/nl_usb_core.h"
#include "globals.h"

/** USB controller used by this driver */
#if USBA_PORT_FOR_MIDI == 0
#define LPC_USBA LPC_USB0
void USB0_IRQHandler(void);
#else
#define LPC_USBA LPC_USB1
void USB1_IRQHandler(void);
#endif

/* USB Core Functions */
void     USBA_Core_Init(void);
void     USBA_Core_Endpoint_Callback_Set(uint8_t ep, EndpointCallback cb);
uint8_t  USBA_Core_IsConfigured(void);
uint8_t  USBA_Core_ReadyToWrite(uint8_t epnum);
uint32_t USBA_Core_BytesToSend(uint32_t endbuff, uint32_t ep);
void     USBA_Core_ForceFullSpeed(void);

void USBA_Core_Interface_Event_Handler_Set(InterfaceEventHandler ievh);
void USBA_Core_Class_Request_Handler_Set(ClassRequestHandler csrqh);
void USBA_Core_SOF_Event_Handler_Set(SOFHandler sofh);

/* Assigning descriptors to the driver */
void USBA_Core_Device_Descriptor_Set(const uint8_t* ddesc);
void USBA_Core_Device_FS_Descriptor_Set(const uint8_t* fsdesc);
void USBA_Core_Device_HS_Descriptor_Set(const uint8_t* hsdesc);
void USBA_Core_Device_String_Descriptor_Set(const uint8_t* strdesc);
void USBA_Core_Device_Device_Quali_Descriptor_Set(const uint8_t* dqdesc);

uint32_t USBA_WriteEP(uint32_t EPNum, uint8_t* pData, uint32_t cnt);
uint32_t USBA_ReadEP(uint32_t EPNum, uint8_t* pData);
uint32_t USBA_ReadReqEP(uint32_t EPNum, uint8_t* pData, uint32_t len);
void     USBA_ResetEP(uint32_t EPNum);

void USBA_DataInStage(void);
void USBA_DataOutStage(void);
void USBA_StatusInStage(void);
void USBA_StatusOutStage(void);
