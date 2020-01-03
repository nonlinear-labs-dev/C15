/******************************************************************************/
/** @file		nl_usb_descmidi.c
    @date		2013-01-23
    @brief    	Descriptors for the USB-MIDI driver
	@ingroup	nl_drv_modules
    @author		Nemanja Nikodijevic [2013-01-23]
*******************************************************************************/
#include "cmsis/LPC43xx.h"
#include "cmsis/lpc_types.h"

#include "usb/nl_usbd.h"
#include "usb/nl_usb_core.h"
#include "usb/nl_usb_midi.h"
#include "usb/nl_usb_descmidi.h"

/* USB Standard Device Descriptor */
const uint8_t USB_MIDI_DeviceDescriptor[] = {
  USB_DEVICE_DESC_SIZE,       /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(BCDUSB_2_0),          /* bcdUSB */
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
const uint8_t USB_MIDI_FSConfigDescriptor[] = {
  /** Configuration 1 */
  USB_CONFIGURATION_DESC_SIZE,       /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(CONFIG_BLOCK_SIZE),
  0x02,                     /* bNumInterfaces */
  0x01,                     /* bConfigurationValue: 0x01 is used to select this configuration */
  0x00,                     /* iConfiguration: no string to describe this configuration */
  USB_CONFIG_BUS_POWERED,   /* bmAttributes */
  USB_CONFIG_POWER_MA(100), /* bMaxPower, device power consumption is 100 mA */
                            /** Interface 0: Standard Audio Control interface */
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
  USB_CS_AUDIO_CONTROL_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_MIDI_HEADER_SUBTYPE,
  WBVAL(BCDADC_1_0),
  WBVAL(USB_CS_AUDIO_CONTROL_TOTAL),
  0x01,
  0x01,
  /** Interface 1: standard midistreaming interface descriptor */
  USB_INTERFACE_DESC_SIZE,
  USB_INTERFACE_DESCRIPTOR_TYPE,
  0x01,                        /* bInterfaceNumber: 1 */
  0x00,                        /* bAlternateSetting */
  0x02,                        /* bNumEndPoints: 2 */
  USB_DEVICE_CLASS_AUDIO,      /* bInterfaceClass: Audio */
  USB_SUBCLASS_MIDI_STREAMING, /* bInterfaceSubclass: MIDI Streaming */
  0x00,                        /* bInterfaceProtocol: no protocol */
  0x00,                        /* iInterface: no string desc */
                               /** CS Interface descriptor - MIDI Streaming */
  USB_CS_MIDI_STREAMING_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_MIDI_HEADER_SUBTYPE,
  WBVAL(BCDADC_1_0),
  WBVAL(USB_CS_MIDI_STREAMING_TOTAL),
  /** MIDI IN jack - embedded */
  USB_MIDI_IN_JACK_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_MIDI_IN_JACK_SUBTYPE,
  USB_MIDI_JACK_EMBEDDED,
  0x01,
  0x00,
  /** MIDI IN jack - external */
  USB_MIDI_IN_JACK_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_MIDI_IN_JACK_SUBTYPE,
  USB_MIDI_JACK_EXTERNAL,
  0x02,
  0x00,
  /** MIDI OUT jack - embedded */
  USB_MIDI_OUT_JACK_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_MIDI_OUT_JACK_SUBTYPE,
  USB_MIDI_JACK_EMBEDDED,
  0x03,
  0x01,
  0x02,
  0x01,
  0x00,
  /** MIDI OUT jack - external */
  USB_MIDI_OUT_JACK_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_MIDI_OUT_JACK_SUBTYPE,
  USB_MIDI_JACK_EXTERNAL,
  0x04,
  0x01,
  0x01,
  0x01,
  0x00,
  /** Bulk OUT - standard endpoint descriptor */
  USB_AUDIO_ENDPOINT_DESC_SIZE,
  USB_ENDPOINT_DESCRIPTOR_TYPE,
  USB_ENDPOINT_OUT(MIDI_EP_OUT),
  USB_ENDPOINT_TYPE_BULK,
  WBVAL(USB_FS_BULK_SIZE),
  0x00,
  0x00,
  0x00,
  /** Bulk OUT - class-specific endpoint descriptor */
  USB_MIDI_ENDPOINT_DESC_SIZE,
  USB_CS_ENDPOINT_DESC_TYPE,
  USB_MIDI_EP_GENERAL_SUBTYPE,
  0x01,
  0x01,
  /** BULK IN - standard endpoint descriptor */
  USB_AUDIO_ENDPOINT_DESC_SIZE,
  USB_ENDPOINT_DESCRIPTOR_TYPE,
  USB_ENDPOINT_IN(MIDI_EP_IN),
  USB_ENDPOINT_TYPE_BULK,
  WBVAL(USB_FS_BULK_SIZE),
  0x00,
  0x00,
  0x00,
  /** Bulk IN - class-specific endpoint descriptor */
  USB_MIDI_ENDPOINT_DESC_SIZE,
  USB_CS_ENDPOINT_DESC_TYPE,
  USB_MIDI_EP_GENERAL_SUBTYPE,
  0x01,
  0x03,
  /* Terminator */
  0
};

/* USB HSConfiguration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t USB_MIDI_HSConfigDescriptor[] = {
  /** Configuration 1 */
  USB_CONFIGURATION_DESC_SIZE,       /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(CONFIG_BLOCK_SIZE),
  0x02,                     /* bNumInterfaces */
  0x01,                     /* bConfigurationValue: 0x01 is used to select this configuration */
  0x00,                     /* iConfiguration: no string to describe this configuration */
  USB_CONFIG_BUS_POWERED,   /* bmAttributes */
  USB_CONFIG_POWER_MA(100), /* bMaxPower, device power consumption is 100 mA */
                            /** Interface 0: Standard Audio Control interface */
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
  USB_CS_AUDIO_CONTROL_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_MIDI_HEADER_SUBTYPE,
  WBVAL(BCDADC_1_0),
  WBVAL(USB_CS_AUDIO_CONTROL_TOTAL),
  0x01,
  0x01,
  /** Interface 1: standard midistreaming interface descriptor */
  USB_INTERFACE_DESC_SIZE,
  USB_INTERFACE_DESCRIPTOR_TYPE,
  0x01,                        /* bInterfaceNumber: 1 */
  0x00,                        /* bAlternateSetting */
  0x02,                        /* bNumEndPoints: 2 */
  USB_DEVICE_CLASS_AUDIO,      /* bInterfaceClass: Audio */
  USB_SUBCLASS_MIDI_STREAMING, /* bInterfaceSubclass: MIDI Streaming */
  0x00,                        /* bInterfaceProtocol: no protocol */
  0x00,                        /* iInterface: no string desc */
                               /** CS Interface descriptor - MIDI Streaming */
  USB_CS_MIDI_STREAMING_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_MIDI_HEADER_SUBTYPE,
  WBVAL(BCDADC_1_0),
  WBVAL(USB_CS_MIDI_STREAMING_TOTAL),
  /** MIDI IN jack - embedded */
  USB_MIDI_IN_JACK_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_MIDI_IN_JACK_SUBTYPE,
  USB_MIDI_JACK_EMBEDDED,
  0x01,
  0x00,
  /** MIDI IN jack - external */
  USB_MIDI_IN_JACK_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_MIDI_IN_JACK_SUBTYPE,
  USB_MIDI_JACK_EXTERNAL,
  0x02,
  0x00,
  /** MIDI OUT jack - embedded */
  USB_MIDI_OUT_JACK_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_MIDI_OUT_JACK_SUBTYPE,
  USB_MIDI_JACK_EMBEDDED,
  0x03,
  0x01,
  0x02,
  0x01,
  0x00,
  /** MIDI OUT jack - external */
  USB_MIDI_OUT_JACK_SIZE,
  USB_CS_INTERFACE_DESC_TYPE,
  USB_MIDI_OUT_JACK_SUBTYPE,
  USB_MIDI_JACK_EXTERNAL,
  0x04,
  0x01,
  0x01,
  0x01,
  0x00,
  /** Bulk OUT - standard endpoint descriptor */
  USB_AUDIO_ENDPOINT_DESC_SIZE,
  USB_ENDPOINT_DESCRIPTOR_TYPE,
  USB_ENDPOINT_OUT(MIDI_EP_OUT),
  USB_ENDPOINT_TYPE_BULK,
  WBVAL(USB_HS_BULK_SIZE),
  0x00,
  0x00,
  0x00,
  /** Bulk OUT - class-specific endpoint descriptor */
  USB_MIDI_ENDPOINT_DESC_SIZE,
  USB_CS_ENDPOINT_DESC_TYPE,
  USB_MIDI_EP_GENERAL_SUBTYPE,
  0x01,
  0x01,
  /** BULK IN - standard endpoint descriptor */
  USB_AUDIO_ENDPOINT_DESC_SIZE,
  USB_ENDPOINT_DESCRIPTOR_TYPE,
  USB_ENDPOINT_IN(MIDI_EP_IN),
  USB_ENDPOINT_TYPE_BULK,
  WBVAL(USB_HS_BULK_SIZE),
  0x00,
  0x00,
  0x00,
  /** Bulk IN - class-specific endpoint descriptor */
  USB_MIDI_ENDPOINT_DESC_SIZE,
  USB_CS_ENDPOINT_DESC_TYPE,
  USB_MIDI_EP_GENERAL_SUBTYPE,
  0x01,
  0x03,
  /* Terminator */
  0
};

/* USB String Descriptor (optional) */
const uint8_t USB_MIDI_StringDescriptor[] = {
  /* Index 0x00: LANGID Codes */
  0x04,                           /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,     /* bDescriptorType */
  WBVAL(0x0409), /* US English */ /* wLANGID */
  /* Index 0x01: Manufacturer */
  (13 * 2 + 2),               /* bLength (13 Char + Type + lenght) */
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
  (7 * 2 + 2),                /* bLength ( 7 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
  'E', 0,
  'M', 0,
  'P', 0,
  'H', 0,
  'A', 0,
  'S', 0,
  'E', 0
};

/* USB Device Qualifier */
const uint8_t USB_MIDI_DeviceQualifier[] = {
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
