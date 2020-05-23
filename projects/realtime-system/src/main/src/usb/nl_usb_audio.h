/******************************************************************************/
/** @file		nl_usb_audio.h
    @date		2015-07-23
    @brief    	Definitions for the USB Audio driver
    @author		Nemanja Nikodijevic 2015-07-23
*******************************************************************************/
#pragma once

/** Audio Request codes */
#define USB_AUDIO_REQ_SET_CUR 0x01
#define USB_AUDIO_REQ_GET_CUR 0x81
#define USB_AUDIO_REQ_SET_MIN 0x02
#define USB_AUDIO_REQ_GET_MIN 0x82
#define USB_AUDIO_REQ_SET_MAX 0x03
#define USB_AUDIO_REQ_GET_MAX 0x83
#define USB_AUDIO_REQ_SET_RES 0x04
#define USB_AUDIO_REQ_GET_RES 0x84

/** Audio Control codes */
#define USB_AUDIO_MUTE_CTRL   0x01
#define USB_AUDIO_VOLUME_CTRL 0x02

/* Definition for Audio Receive Callback function */
typedef void (*AudioRcvCallback)(uint32_t* buff, uint32_t len);

/* USB Audio functions */
void USB_Audio_Init(AudioRcvCallback audrcv, LPC_TIMERn_Type* TIMx);
void USB_Audio_Poll(void);
