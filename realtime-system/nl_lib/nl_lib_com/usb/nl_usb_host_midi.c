/******************************************************************************/
/** @file		nl_usb_host_midi.c
    @date		2015-11-15
    @brief    	Driver for the MIDI USB host port on LPC43xx
    @author		Nemanja Nikodijevic 2015-11-15
*******************************************************************************/
#include "usb/nl_usb_host.h"
#include "usb/nl_usb_host_midi.h"

#define USBH_MIDI_MAX_BUFF 512

static uint8_t  midi_in_ep = 0, midi_out_ep = 0;
static uint16_t midi_in_len = 0, midi_out_len = 0;

static MIDI_Message_Callback MIDI_MsgCb = 0;

static QH_T    midi_in_QH, midi_out_QH;
static QTD_T   midi_in_TD, midi_out_TD;
static uint8_t midi_in_buff[USBH_MIDI_MAX_BUFF];

static void USBH_MIDI_Process_Buffer(uint16_t len)
{
  MIDI_Message msg;
  uint16_t     i;

  for (i = 0; i < len; i++)
  {
    switch (midi_in_buff[i] & 0x0F)
    {
      case 0x08:
      case 0x09:
      case 0x0A:
      case 0x0B:
      case 0x0E:
        msg.m_status = midi_in_buff[i + 1];
        msg.m_data0  = midi_in_buff[i + 2];
        msg.m_data1  = midi_in_buff[i + 3];
        if (MIDI_MsgCb)
          MIDI_MsgCb(&msg);
        i += 3;
        break;
      case 0x03:
      case 0x04:
      case 0x07:
        i += 3;
        break;
      case 0x02:
      case 0x06:
      case 0x0C:
      case 0x0D:
        i += 2;
        break;
      case 0x05:
      case 0x0F:
        i++;
        break;
      default:
        return;
    }
  }
}

uint8_t USBH_MIDI_Parse_Config(uint8_t* desc, uint16_t len)
{
  uint8_t* dptr       = desc;
  uint8_t  midi_found = 0;

  while (dptr != (desc + *(uint16_t*) (desc + 2)))
  {
    switch (dptr[1])
    {
      case USBH_DESC_INTERFACE:
        if (dptr[5] == 0x01 && dptr[6] == 0x03 && dptr[7] == 0x00)
        {
          midi_found = 1;
          dptr += dptr[0];
        }
        else
        {
          dptr += dptr[0];
        }
        break;
      case USBH_DESC_ENDPOINT:
        if ((dptr[3] & 0x03) == 0x02)
        {
          if (dptr[2] & 0x80)
          {
            midi_in_ep  = dptr[2] & 0x7F;
            midi_in_len = *((uint16_t*) (dptr + 4));
            dptr += dptr[0];
          }
          else
          {
            midi_out_ep  = dptr[2] & 0x7F;
            midi_out_len = *((uint16_t*) (dptr + 4));
            dptr += dptr[0];
          }
        }
        else
          dptr += dptr[0];
        break;
      default:
        dptr += dptr[0];
        break;
    }
  }
  if (midi_found)
    return 1;
  else
    return 0;
}

void USBH_MIDI_Prepare_Endpoints(void)
{
  USB_Host_QH_Init(&midi_in_QH, midi_in_ep, midi_in_len);
  USB_Host_QH_Init(&midi_out_QH, midi_out_ep, midi_out_len);

  USB_Host_QH_Insert(&midi_in_QH);
  USB_Host_QH_Insert(&midi_out_QH);

  USB_Host_PrepareTD(&midi_in_TD, USBH_DIR_IN, midi_in_buff, midi_in_len);
  midi_in_QH.next_qTD = (uint32_t) &midi_in_TD;
}

void USBH_MIDI_Remove_Device(void)
{
  USB_Host_QH_Remove(&midi_in_QH);
  USB_Host_QH_Remove(&midi_out_QH);
}

void USBH_MIDI_QH_Process(void)
{
  uint16_t in_len = 0;

  if (midi_in_TD.total_bytes & USBH_TD_STATUS_ACTIVE)
    return;
  in_len = midi_in_len - (midi_in_TD.total_bytes >> 16);

  USBH_MIDI_Process_Buffer(in_len);

  USB_Host_PrepareTD(&midi_in_TD, USBH_DIR_IN, midi_in_buff, midi_in_len);
  midi_in_QH.next_qTD = (uint32_t) &midi_in_TD;
}

void USBH_MIDI_Set_Callback(MIDI_Message_Callback msg_cb)
{
  MIDI_MsgCb = msg_cb;
}
