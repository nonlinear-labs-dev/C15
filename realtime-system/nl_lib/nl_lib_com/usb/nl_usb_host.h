/******************************************************************************/
/** @file		nl_usb_host.h
    @date		2015-10-15
    @brief    	Driver for the USB host port on LPC43xx
    @author		Nemanja Nikodijevic 2015-10-15
*******************************************************************************/
#ifndef _NL_USB_HOST_H_
#define _NL_USB_HOST_H_

#include <stdint.h>
#include "cmsis/LPC43xx.h"

#define USB_MIDI_HOST

/** bit defines for USBCMD_H register
 * @{
 */
/** Run/Stop */
#define USBH_USBCMD_RS (1 << 0)
/** Controller Reset */
#define USBH_USBCMD_RST (1 << 1)
/** Periodic Schedule Enable */
#define USBH_USBCMD_PSE (1 << 4)
/** Async Schedule Enable */
#define USBH_USBCMD_ASE (1 << 5)
/** @} */

/** bit defines for USBSTS_H register
 * @{
 */
/** USB Interrupt */
#define USBH_USBSTS_UI (1 << 0)
/** USB Error Interrupt */
#define USBH_USBSTS_UEI (1 << 1)
/** Port change detect */
#define USBH_USBSTS_PCI (1 << 2)
/** Frame list roll-over */
#define USBH_USBSTS_FRI (1 << 3)
/** System Error */
#define USBH_USBSTS_SEI (1 << 4)
/** Async advance interrupt */
#define USBH_USBSTS_AAI (1 << 5)
/** Start-of-Frame received */
#define USBH_USBSTS_SRI (1 << 7)
/** Host controller halted status */
#define USBH_USBSTS_HCH (1 << 12)
/** Reclamation status */
#define USBH_USBSTS_RCL (1 << 13)
/** Periodic schedule status */
#define USBH_USBSTS_PS (1 << 14)
/** Async schedule status */
#define USBH_USBSTS_AS (1 << 15)
/** USB Host Async interrupt */
#define USBH_USBSTS_UAI (1 << 18)
/** USB Host Periodic interrupt */
#define USBH_USBSTS_UPI (1 << 19)
/** @} */

/** bit defines for USBMODE_H register
 * @{
 */
/** Controller mode: IDLE */
#define USBH_USBMODE_CM_IDLE (0x0 << 0)
/** Controller mode: HOST */
#define USBH_USBMODE_CM_HOST (0x3 << 0)
/** Stream Disable mode */
#define USBH_USBMODE_SDIS (1 << 4)
/** VBUS power select: HIGH */
#define USBH_USBMODE_VBPS_HI (1 << 5)
/** VBUS power select: LOW */
#define USBH_USBMODE_VBPS_LO (0 << 5)
/** @} */

/** bit defines for PORTSC1_H register
 * @{
 */
/** Current connect status */
#define USBH_PORTSC_CCS (1 << 0)
/** Connect status change */
#define USBH_PORTSC_CSC (1 << 1)
/** Port enable */
#define USBH_PORTSC_PE (1 << 2)
/** Port disable/enable change */
#define USBH_PORTSC_PEC (1 << 3)
/** Port reset */
#define USBH_PORTSC_PR (1 << 8)
/** Port power */
#define USBH_PORTSC_PP (1 << 12)
/** @} */

typedef volatile struct
{
  volatile uint32_t next_qTD;
  volatile uint32_t alt_next_qTD;
  volatile uint32_t total_bytes;
  volatile uint32_t buffer0;
  volatile uint32_t buffer1;
  volatile uint32_t buffer2;
  volatile uint32_t buffer3;
  volatile uint32_t buffer4;
} QTD_T __attribute__((aligned(32)));

typedef volatile struct
{
  volatile uint32_t next_QH;
  volatile uint32_t ep_chars;
  volatile uint32_t ep_caps;
  volatile uint32_t curr_qTD;
  volatile uint32_t next_qTD;
  volatile uint32_t alt_next_qTD;
  volatile uint32_t total_bytes;
  volatile uint32_t buffer0;
  volatile uint32_t buffer1;
  volatile uint32_t buffer2;
  volatile uint32_t buffer3;
  volatile uint32_t buffer4;
  volatile uint32_t padding[4];
} QH_T __attribute__((aligned(64)));

/** defines for device speed fields
 * @{
 */
/** Full Speed */
#define USBH_FULL_SPEED 0
/** Low Speed */
#define USBH_LOW_SPEED 1
/** High Speed */
#define USBH_HIGH_SPEED 2
/** @} */

/** bit defines for Queue Head fields
 * @{
 */
/** Endpoint number */
#define USBH_QH_EP_NUM(x) ((x & 0xF) << 8)
/** Data Toggle Control */
#define USBH_QH_DTC (1 << 14)
/** Max Packet Size */
#define USBH_QH_MAX_PKT(x) (x << 16)
/** Control Endpoint Flag */
#define USBH_QH_CTRL_EP_FLAG (1 << 27)
/** High Bandwidth Pipe Multiplier (Mult) */
#define USBH_QH_MULT(x) (x << 30)
/** @} */

/** defines for PID Code (transfer direction) fields
 * @{
 */
/** OUT token */
#define USBH_DIR_OUT 0
/** IN token */
#define USBH_DIR_IN 1
/** SETUP token */
#define USBH_DIR_SETUP 2
/** @} */

/** qTD token Status - Halted */
#define USBH_TD_STATUS_HALTED (1 << 6)
/** qTD token Status - Active */
#define USBH_TD_STATUS_ACTIVE (1 << 7)
/** qTD token field define IOC */
#define USBH_TD_IOC (1 << 15)
/** qTD token field define DT */
#define USBH_TD_DATA_TOGGLE (1 << 31)

/** defines for the USB request type
 * @{
 */
/** Direction: Host-to-Device */
#define USBH_REQ_DIR_H2D (0 << 7)
/** Direction: Device-to-Host */
#define USBH_REQ_DIR_D2H (1 << 7)
/** Type: Standard */
#define USBH_REQ_TYP_STD (0 << 5)
/** Type: Class-specific */
#define USBH_REQ_TYP_CLASS (1 << 5)
/** Recipient: Device */
#define USBH_REQ_DEVICE (0 << 0)
/** Recipient: Interface */
#define USBH_REQ_INTERFACE (1 << 0)
/** Recipient: Endpoint */
#define USBH_REQ_ENDPOINT (2 << 0)
/** Recipient: Other */
#define USBH_REQ_OTHER (3 << 0)
/** @} */

/** defines for the USB request codes
 * @{
 */
#define USBH_REQ_SET_ADDRESS       5
#define USBH_REQ_GET_DESCRIPTOR    6
#define USBH_REQ_SET_CONFIGURATION 9
#define USBH_REQ_SET_INTERFACE     11
/** @} */

/** defines for the USB descriptor types
 * @{
 */
#define USBH_DESC_DEVICE        1
#define USBH_DESC_CONFIGURATION 2
#define USBH_DESC_STRING        3
#define USBH_DESC_INTERFACE     4
#define USBH_DESC_ENDPOINT      5
/** @} */

/** Queue Link Terminate  */
#define USBH_LINK_TERMINATOR 1

void USB_Host_Init(void);
void USB_Host_Task(void);

void USB_Host_QH_Init(QH_T* qh, uint8_t epnum, uint16_t maxpkt);
void USB_Host_QH_Insert(QH_T* qh);
void USB_Host_QH_Remove(QH_T* qh);

void USB_Host_PrepareTD(QTD_T* qTD, uint8_t dir, uint8_t* buffer, uint16_t length);

#endif
