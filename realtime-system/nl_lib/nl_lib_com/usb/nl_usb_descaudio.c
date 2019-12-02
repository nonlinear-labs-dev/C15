/******************************************************************************/
/** @file		nl_usb_descaudio.c
    @date		2015-07-23
    @brief    	Descriptors for the USB Audio driver
    @author		Nemanja Nikodijevic 2015-07-23
*******************************************************************************/
#include "cmsis/LPC43xx.h"
#include "cmsis/lpc_types.h"

#include "usb/nl_usbd.h"
#include "usb/nl_usb_core.h"
#include "usb/nl_usb_descaudio.h"

/* USB Standard Device Descriptor */
const uint8_t USB_Audio_DeviceDescriptor[] = {
  USB_DEVICE_DESC_SIZE,       /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(0x0110),              //BCDUSB_2_0),			/* bcdUSB */
  0x00,                       /* bDeviceClass */
  0x00,                       /* bDeviceSubClass */
  0x00,                       /* bDeviceProtocol */
  USB_MAX_PACKET0,            /* bMaxPacketSize */
  WBVAL(VENDOR_ID),           /* idVendor */
  WBVAL(PRODUCT_ID),          /* idProduct */
  WBVAL(BCD_DEVICE),          /* bcdDevice */
  0x01,                       /* iManufacturer */
  0x02,                       /* iProduct */
  0x00,                       /* iSerialNumber */
  0x01,                       /* bNumConfigurations */
};

/** USB FSConfiguration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class */
const uint8_t USB_Audio_FSConfigDescriptor[] = {
  /** Configuration Descriptor */
  USB_CONFIGURATION_DESC_SIZE,       /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(AUDIO_CONFIG_BLOCK_SIZE),    /* wTotalLength */
#ifndef USB_AUDIO_OUTPUT_ONLY
  0x03, /* bNumInterfaces */
#else
  0x02,
#endif
  0x01,                     /* bConfigurationValue */
  0x00,                     /* iConfiguration - no string desc */
  USB_CONFIG_BUS_POWERED,   /* bmAttributes */
  USB_CONFIG_POWER_MA(100), /* bMaxPower, device power consumption is 100 mA */
                            /** Interface 0: Standard AC interface */
  USB_INTERFACE_DESC_SIZE,
  USB_INTERFACE_DESCRIPTOR_TYPE,
  0x00,                       /* bInterfaceNumber: 0 */
  0x00,                       /* bAlternateSetting */
  0x00,                       /* bNumEndPoints: no endpoints */
  USB_DEVICE_CLASS_AUDIO,     /* bInterfaceClass: Audio */
  USB_SUBCLASS_AUDIO_CONTROL, /* bInterfaceSubClass: AudioControl */
  0x00,                       /* bInterfaceProtocol: no protocol */
  0x00,                       /* iInterface: no string desc */
/** CS Interface descriptor - Audio Control */
#ifndef USB_AUDIO_OUTPUT_ONLY
  (USB_CS_AUDIO_CONTROL_SIZE + 1), /** two interfaces */
  USB_CS_INTERFACE_DESC_TYPE,
  USB_AUDIO_CONTROL_HEADER_SUBTYPE,
  WBVAL(BCDADC_1_0),
  WBVAL(USB_CS_AUDIO_CONTROL_TOTAL),
  0x02,
  0x01,
  0x02,
#else
  USB_CS_AUDIO_CONTROL_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_AUDIO_CONTROL_HEADER_SUBTYPE,
  WBVAL(BCDADC_1_0),
  WBVAL(USB_CS_AUDIO_CONTROL_TOTAL),
  0x01,
  0x01,
#endif
#ifndef USB_AUDIO_OUTPUT_ONLY
  /** Input Terminal descriptor - Line In */
  USB_CS_AUDIO_INPUT_TERMINAL_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_AUDIO_INPUT_TERMINAL_SUBTYPE,
  0x01, /* bTerminalID */
  WBVAL(USB_AUDIO_LINE_CONN_TERMINAL),
  0x00,          /* bAssocTerminal */
  0x02,          /* bNrChannels */
  WBVAL(0x0003), /* wChannelConfig - Stereo L and R */
  0x00,          /* iChannelNames */
  0x00,          /* iTerminal */
#endif
  /** Input Terminal descriptor - USB Stream */
  USB_CS_AUDIO_INPUT_TERMINAL_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_AUDIO_INPUT_TERMINAL_SUBTYPE,
  0x02, /* bTerminalID */
  WBVAL(USB_AUDIO_USB_STREAM_TERMINAL),
  0x00,          /* bAssocTerminal */
  0x02,          /* bNrChannels */
  WBVAL(0x0003), /* wChannelConfig - Stereo L and R */
  0x00,          /* iChannelNames */
  0x00,          /* iTerminal */
                 /** Output Terminal descriptor - Speaker */
  USB_CS_AUDIO_OUTPUT_TERMINAL_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_AUDIO_OUTPUT_TERMINAL_SUBTYPE,
  0x03, /* bTerminalID */
  WBVAL(USB_AUDIO_SPEAKER_TERMINAL),
  0x00, /* bAssocTerminal */
  0x05, /* bSourceID - From Feature Unit 0x05 */
  0x00, /* iTerminal */
#ifndef USB_AUDIO_OUTPUT_ONLY
  /** Output Terminal descriptor - USB Stream */
  USB_CS_AUDIO_OUTPUT_TERMINAL_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_AUDIO_OUTPUT_TERMINAL_SUBTYPE,
  0x04, /* bTerminalID */
  WBVAL(USB_AUDIO_USB_STREAM_TERMINAL),
  0x00, /* bAssocTerminal */
  0x01, /* bSourceID - From IT Line In */
  0x00, /* iTerminal */
#endif
  /** Feature Unit descriptor - OUT */
  USB_CS_AUDIO_FEATURE_UNIT_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_AUDIO_FEATURE_UNIT_SUBTYPE,
  0x05, /* bTerminalID */
  0x02, /* bSourceID - From IT USB Stream */
  0x01, /* bControlSize */
  0x01, /* mute control */
  0x02, /* vol 1 */
  0x02, /* vol 2 */
  0x00, /* iFeature */
#ifndef USB_AUDIO_OUTPUT_ONLY
  /** Interface 1: Standard AS interface - Alt setting 0 */
  USB_INTERFACE_DESC_SIZE,
  USB_INTERFACE_DESCRIPTOR_TYPE,
  USB_AUDIO_INPUT_INTERFACE,    /* bInterfaceNumber: 1 */
  0x00,                         /* bAlternateSetting */
  0x00,                         /* bNumEndPoints: no endpoints */
  USB_DEVICE_CLASS_AUDIO,       /* bInterfaceClass: Audio */
  USB_SUBCLASS_AUDIO_STREAMING, /* bInterfaceSubClass: AudioStreaming */
  0x00,                         /* bInterfaceProtocol: no protocol */
  0x00,                         /* iInterface: no string desc */
                                /** Interface 1: Standard AS interface - Alt setting 1 */
  USB_INTERFACE_DESC_SIZE,
  USB_INTERFACE_DESCRIPTOR_TYPE,
  USB_AUDIO_INPUT_INTERFACE,    /* bInterfaceNumber: 1 */
  0x01,                         /* bAlternateSetting */
  0x01,                         /* bNumEndPoints: 1 endpoint */
  USB_DEVICE_CLASS_AUDIO,       /* bInterfaceClass: Audio */
  USB_SUBCLASS_AUDIO_STREAMING, /* bInterfaceSubClass: AudioStreaming */
  0x00,                         /* bInterfaceProtocol: no protocol */
  0x00,                         /* iInterface: no string desc */
                                /** CS Interface descriptor - Audio Streaming */
  USB_CS_AUDIO_STREAMING_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_AUDIO_STREAMING_GENERAL_SUBTYPE,
  0x04,          /* bTerminalLink - Output Terminal USB Stream */
  0x00,          /* bDelay */
  WBVAL(0x0001), /* wFormatTag - PCM */
                 /** Format Type Descriptor */
  USB_AUDIO_FORMAT_TYPE_I_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_AUDIO_FORMAT_TYPE_SUBTYPE,
  0x01,          /* bFormatType I */
  0x02,          /* bNrChannels */
  0x03,          /* bSubframeSize */
  0x18,          /* bBitResolution - 24-bit */
  0x01,          /* bSamFreqType */
  B3VAL(0xBB80), /* tSamFreq [0] - 48000 */
                 /** Endpoint 2 descriptor - Isochronous IN */
  USB_AUDIO_ENDPOINT_DESC_SIZE,
  USB_ENDPOINT_DESCRIPTOR_TYPE,
  USB_ENDPOINT_IN(0x02),
  (USB_ENDPOINT_TYPE_ISOCHRONOUS | USB_ENDPOINT_SYNC_ASYNCHRONOUS),
  WBVAL(0x120), /* wMaxPacketSize - 288 (6*48) */
  0x01,         /* bInterval */
  0x00,
  0x00,
  /** CS Audio Data Endpoint descriptor */
  USB_AUDIO_DATA_ENDPOINT_SIZE,
  USB_CS_ENDPOINT_DESC_TYPE,
  USB_AUDIO_STREAMING_GENERAL_SUBTYPE,
  0x01,          /* bmAttributes */
  0x00,          /* bLockDelayUnits */
  WBVAL(0x0000), /* wLockDelay */
#endif
  /** Interface 2: Standard AS interface - Alt setting 0 */
  USB_INTERFACE_DESC_SIZE,
  USB_INTERFACE_DESCRIPTOR_TYPE,
  USB_AUDIO_OUTPUT_INTERFACE,   /* bInterfaceNumber: 2 */
  0x00,                         /* bAlternateSetting */
  0x00,                         /* bNumEndPoints: no endpoints */
  USB_DEVICE_CLASS_AUDIO,       /* bInterfaceClass: Audio */
  USB_SUBCLASS_AUDIO_STREAMING, /* bInterfaceSubClass: AudioStreaming */
  0x00,                         /* bInterfaceProtocol: no protocol */
  0x00,                         /* iInterface: no string desc */
                                /** Interface 2: Standard AS interface - Alt setting 1 */
  USB_INTERFACE_DESC_SIZE,
  USB_INTERFACE_DESCRIPTOR_TYPE,
  USB_AUDIO_OUTPUT_INTERFACE,   /* bInterfaceNumber: 2 */
  0x01,                         /* bAlternateSetting */
  0x02,                         /* bNumEndPoints: 2 endpoints */
  USB_DEVICE_CLASS_AUDIO,       /* bInterfaceClass: Audio */
  USB_SUBCLASS_AUDIO_STREAMING, /* bInterfaceSubClass: AudioStreaming */
  0x00,                         /* bInterfaceProtocol: no protocol */
  0x00,                         /* iInterface: no string desc */
                                /** CS Interface descriptor - Audio Streaming */
  USB_CS_AUDIO_STREAMING_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_AUDIO_STREAMING_GENERAL_SUBTYPE,
  0x02,          /* bTerminalLink - Input Terminal USB Stream */
  0x00,          /* bDelay */
  WBVAL(0x0001), /* wFormatTag - PCM */
                 /** Format Type Descriptor */
  USB_AUDIO_FORMAT_TYPE_I_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_AUDIO_FORMAT_TYPE_SUBTYPE,
  0x01,          /* bFormatType I */
  0x02,          /* bNrChannels */
  0x03,          /* bSubframeSize */
  0x18,          /* bBitResolution - 24-bit */
  0x01,          /* bSamFreqType */
  B3VAL(0xBB80), /* tSamFreq [0] - 48000 */
                 /** Endpoint 1 descriptor - Isochronous OUT */
  USB_AUDIO_ENDPOINT_DESC_SIZE,
  USB_ENDPOINT_DESCRIPTOR_TYPE,
  USB_ENDPOINT_OUT(0x01),
  (USB_ENDPOINT_TYPE_ISOCHRONOUS | USB_ENDPOINT_SYNC_ASYNCHRONOUS),
  WBVAL(USB_AUDIO_ISO_MAX_PKT), /* wMaxPacketSize - 582 (6*97) */
  0x01,                         /* bInterval */
  0x00,
  0x81, /* bSynchAddress */
        /** CS Audio Data Endpoint descriptor */
  USB_AUDIO_DATA_ENDPOINT_SIZE,
  USB_CS_ENDPOINT_DESC_TYPE,
  USB_AUDIO_STREAMING_GENERAL_SUBTYPE,
  0x01,          /* bmAttributes */
  0x02,          /* bLockDelayUnits */
  WBVAL(0x0000), /* wLockDelay */
                 /** Endpoint 1 descriptor - Isochronous IN - feedback ep */
  USB_AUDIO_ENDPOINT_DESC_SIZE,
  USB_ENDPOINT_DESCRIPTOR_TYPE,
  USB_ENDPOINT_IN(0x01),
  (USB_ENDPOINT_TYPE_ISOCHRONOUS),
  WBVAL(0x0003), /* wMaxPacketSize - 582 (6*97) */
  0x01,          /* bInterval */
  0x01,          /* bRefresh */
  0x00,
};

/* USB HSConfiguration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t* USB_Audio_HSConfigDescriptor = USB_Audio_FSConfigDescriptor;

/* USB String Descriptor (optional) */
const uint8_t USB_Audio_StringDescriptor[] = {
  /* Index 0x00: LANGID Codes */
  0x04,                           /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,     /* bDescriptorType */
  WBVAL(0x0409), /* US English */ /* wLANGID */
  /* Index 0x01: Manufacturer */
  (13 * 2 + 2),               /* bLength (13 Char + Type + length) */
  USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
  'N', 0,
  'o', 0,
  'n', 0,
  'l', 0,
  'i', 0,
  'n', 0,
  'e', 0,
  'a', 0,
  'r', 0,
  'L', 0,
  'a', 0,
  'b', 0,
  's', 0,
  /* Index 0x02: Product */
  (12 * 2 + 2),               /* bLength ( 12 Char + Type + length) */
  USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
  'N', 0,
  'L', 0,
  ' ', 0,
  'S', 0,
  'o', 0,
  'u', 0,
  'n', 0,
  'd', 0,
  'c', 0,
  'a', 0,
  'r', 0,
  'd', 0
};

/* USB Device Qualifier */
const uint8_t USB_Audio_DeviceQualifier[] = {
  USB_DEVICE_QUALI_SIZE,                /* bLength */
  USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(0x0200), /* 2.00 */             /* bcdUSB */
  0x00,                                 /* bDeviceClass */
  0x00,                                 /* bDeviceSubClass */
  0x00,                                 /* bDeviceProtocol */
  USB_MAX_PACKET0,                      /* bMaxPacketSize0 */
  0x01,                                 /* bNumOtherSpeedConfigurations */
  0x00                                  /* bReserved */
};
