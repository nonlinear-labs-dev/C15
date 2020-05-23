/******************************************************************************/
/** @file		nl_usb_descmidi.h
    @date		2013-01-23
    @brief    	Descriptors header for the USB-MIDI driver
    @example
    @ingroup  	USB_MIDI
    @author		Nemanja Nikodijevic 2013-01-23
*******************************************************************************/
#pragma once

/** MIDI device descriptor fields
 * @{
 */
/** bcdUSB 2.0 */
#define BCDUSB_2_0 0x0200
/** Vendor Id */
#define VENDOR_ID 0xFFFF
/** Product Id */
#define PRODUCT_ID 0x0005
/** bcdDevice */
#define BCD_DEVICE 0x0001
/** @} */

/** Size of the configuration block */
#define CONFIG_BLOCK_SIZE 0x0065
/** Size of standard Audio endpoint descriptor */
#define USB_AUDIO_ENDPOINT_DESC_SIZE 0x09

/** Audio interface subclasses
 * @{
 */
/** Audio Control subclass */
#define USB_SUBCLASS_AUDIO_CONTROL 0x01
/** MIDI Streaming subclass */
#define USB_SUBCLASS_MIDI_STREAMING 0x03
/** @} */

/** MIDI Streaming class-specific interface descriptor subtypes
 * @{
 */
/** MS_HEADER */
#define USB_MIDI_HEADER_SUBTYPE 0x01
/** MIDI_IN_JACK */
#define USB_MIDI_IN_JACK_SUBTYPE 0x02
/** MIDI_OUT_JACK */
#define USB_MIDI_OUT_JACK_SUBTYPE 0x03
/** @} */

/** Class-specific descriptor types
 * @{
 */
/** Class-specific interface descriptor */
#define USB_CS_INTERFACE_DESC_TYPE 0x24
/** Class-specific endpoint descriptor */
#define USB_CS_ENDPOINT_DESC_TYPE 0x25
/** Class-specific MIDI Streaming general endpoint descriptor subtype */
#define USB_MIDI_EP_GENERAL_SUBTYPE 0x01
/** @} */

/** Audio device class specification that the device complies to */
#define BCDADC_1_0 0x0100

/** Size of class-specific descriptors
 * @{
 */
/** Size of CS Audio Control header interface descriptor */
#define USB_CS_AUDIO_CONTROL_SIZE 0x09
/** Total size of CS Audio Control interface descriptors */
#define USB_CS_AUDIO_CONTROL_TOTAL 0x0009
/** Size of CS MIDI Streaming header interface descriptor */
#define USB_CS_MIDI_STREAMING_SIZE 0x07
/** Total size of CS MIDI Streaming interface descriptors */
#define USB_CS_MIDI_STREAMING_TOTAL 0x0041
/** Size of MIDI IN jack descriptor */
#define USB_MIDI_IN_JACK_SIZE 0x06
/** Size of MIDI OUT jack descriptor */
#define USB_MIDI_OUT_JACK_SIZE 0x09
/** Size of MIDI Streaming endpoint descriptor */
#define USB_MIDI_ENDPOINT_DESC_SIZE 0x05
/** @} */

/** USB MIDI jack types
 * @{
 */
/** Embedded jack */
#define USB_MIDI_JACK_EMBEDDED 0x01
/** External jack */
#define USB_MIDI_JACK_EXTERNAL 0x02
/** @} */

/** Bulk transfer maximum packet size
 * @{
 */
/** Full-speed */
#define USB_FS_BULK_SIZE 0x0040
/** Hi-speed */
#define USB_HS_BULK_SIZE 0x0200
/** @} */

/** MIDI endpoint numbers
 * @{
 */
/** Bulk OUT endpoint - 0x01 */
#define MIDI_EP_OUT 0x01
/** Bulk IN endpoint  - 0x02 */
#define MIDI_EP_IN 0x02
/** @} */

extern const uint8_t USB_MIDI_DeviceDescriptor[];
extern const uint8_t USB_MIDI_FSConfigDescriptor[];
extern const uint8_t USB_MIDI_HSConfigDescriptor[];
extern const uint8_t USB_MIDI_StringDescriptor[];
extern const uint8_t USB_MIDI_DeviceQualifier[];
