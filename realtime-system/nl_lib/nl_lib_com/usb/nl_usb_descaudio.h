/******************************************************************************/
/** @file		nl_usb_descaudio.h
    @date		2015-07-23
    @brief    	Descriptors for the USB Audio driver
    @author		Nemanja Nikodijevic 2015-07-23
*******************************************************************************/

#ifndef _NL_USB_DESCAUDIO_H_
#define _NL_USB_DESCAUDIO_H_

#define USB_AUDIO_OUTPUT_ONLY

/** Audio device descriptor fields
 * @{
 */
/** bcdUSB 2.0 */
#define BCDUSB_2_0 0x0200
/** Vendor Id */
#define VENDOR_ID 0xFFFF
/** Product Id */
#define PRODUCT_ID 0x0009
/** bcdDevice */
#define BCD_DEVICE 0x0001
/** @} */

#define USB_AUDIO_ISO_MAX_PKT 0x126  //0x246

#define USB_AUDIO_OUTPUT_INTERFACE 0x01
#define USB_AUDIO_INPUT_INTERFACE  0x02

/** Size of standard Audio endpoint descriptor */
#define USB_AUDIO_ENDPOINT_DESC_SIZE 0x09

/** Audio interface subclasses
 * @{
 */
/** Audio Control subclass */
#define USB_SUBCLASS_AUDIO_CONTROL 0x01
/** MIDI Streaming subclass */
#define USB_SUBCLASS_AUDIO_STREAMING 0x02
/** @} */

/** Audio class-specific interface descriptor subtypes
 * @{
 */
/** AC_HEADER */
#define USB_AUDIO_CONTROL_HEADER_SUBTYPE 0x01
/** AS_HEADER */
#define USB_AUDIO_STREAMING_GENERAL_SUBTYPE 0x01
/** Format Type */
#define USB_AUDIO_FORMAT_TYPE_SUBTYPE 0x02
/** @} */

/** Class-specific descriptor types
 * @{
 */
/** Class-specific interface descriptor */
#define USB_CS_INTERFACE_DESC_TYPE 0x24
/** Class-specific endpoint descriptor */
#define USB_CS_ENDPOINT_DESC_TYPE 0x25
/** Class-specific Audio Input Terminal descriptor subtype */
#define USB_AUDIO_INPUT_TERMINAL_SUBTYPE 0x02
/** Class-specific Audio Output Terminal descriptor subtype */
#define USB_AUDIO_OUTPUT_TERMINAL_SUBTYPE 0x03
/** Class-specific Audio Feature Unit descriptor subtype */
#define USB_AUDIO_FEATURE_UNIT_SUBTYPE 0x06
/** @} */

/** Audio device class specification that the device complies to */
#define BCDADC_1_0 0x0100

/** Size of class-specific descriptors
 * @{
 */
/** Size of CS Audio Control header interface descriptor */
#define USB_CS_AUDIO_CONTROL_SIZE 0x09
/** Size of CS Audio Control header interface descriptor */
#define USB_CS_AUDIO_STREAMING_SIZE 0x07
/** Size of Input Terminal descriptor */
#define USB_CS_AUDIO_INPUT_TERMINAL_SIZE 0x0C
/** Size of Output Terminal descriptor */
#define USB_CS_AUDIO_OUTPUT_TERMINAL_SIZE 0x09
/** Size of Feature Unit descriptor */
#define USB_CS_AUDIO_FEATURE_UNIT_SIZE 0x0A
/** Total size of CS Audio Control interface descriptors */
#ifndef USB_AUDIO_OUTPUT_ONLY
#define USB_CS_AUDIO_CONTROL_TOTAL (USB_CS_AUDIO_CONTROL_SIZE               \
                                    + 2 * USB_CS_AUDIO_INPUT_TERMINAL_SIZE  \
                                    + 2 * USB_CS_AUDIO_OUTPUT_TERMINAL_SIZE \
                                    + USB_CS_AUDIO_FEATURE_UNIT_SIZE + 1)
#else
#define USB_CS_AUDIO_CONTROL_TOTAL (USB_CS_AUDIO_CONTROL_SIZE           \
                                    + USB_CS_AUDIO_INPUT_TERMINAL_SIZE  \
                                    + USB_CS_AUDIO_OUTPUT_TERMINAL_SIZE \
                                    + USB_CS_AUDIO_FEATURE_UNIT_SIZE)
#endif
/** Format Type I descriptor */
#define USB_AUDIO_FORMAT_TYPE_I_SIZE 0x0B
/** Audio Data Endpoint descriptor size */
#define USB_AUDIO_DATA_ENDPOINT_SIZE 0x07
/** @} */

/** Size of the configuration block */
#ifndef USB_AUDIO_OUTPUT_ONLY
#define AUDIO_CONFIG_BLOCK_SIZE (USB_CONFIGURATION_DESC_SIZE                                                                                                      \
                                 + 5 * USB_INTERFACE_DESC_SIZE                                                                                                    \
                                 + USB_CS_AUDIO_CONTROL_TOTAL                                                                                                     \
                                 + 2 * (USB_CS_AUDIO_STREAMING_SIZE + USB_AUDIO_FORMAT_TYPE_I_SIZE + USB_AUDIO_ENDPOINT_DESC_SIZE + USB_AUDIO_DATA_ENDPOINT_SIZE) \
                                 + USB_AUDIO_ENDPOINT_DESC_SIZE)
#else
#define AUDIO_CONFIG_BLOCK_SIZE (USB_CONFIGURATION_DESC_SIZE        \
                                 + 3 * USB_INTERFACE_DESC_SIZE      \
                                 + USB_CS_AUDIO_CONTROL_TOTAL       \
                                 + (USB_CS_AUDIO_STREAMING_SIZE     \
                                    + USB_AUDIO_FORMAT_TYPE_I_SIZE  \
                                    + USB_AUDIO_ENDPOINT_DESC_SIZE  \
                                    + USB_AUDIO_DATA_ENDPOINT_SIZE) \
                                 + USB_AUDIO_ENDPOINT_DESC_SIZE)
#endif

/** USB Audio Terminal types
 * @{
 */
/** Line Connector */
#define USB_AUDIO_LINE_CONN_TERMINAL 0x0601
/** USB Streaming */
#define USB_AUDIO_SPEAKER_TERMINAL 0x0301
/** USB Streaming */
#define USB_AUDIO_USB_STREAM_TERMINAL 0x0101
/** @} */

extern const uint8_t  USB_Audio_DeviceDescriptor[];
extern const uint8_t  USB_Audio_FSConfigDescriptor[];
extern const uint8_t* USB_Audio_HSConfigDescriptor;
extern const uint8_t  USB_Audio_StringDescriptor[];
extern const uint8_t  USB_Audio_DeviceQualifier[];

#endif
