/******************************************************************************/
/** @file		nl_ehc_adc.h
    @date		2020-01-10
    @version	0
    @author		KSTR
    @brief		ADC access for external controllers (pedals etc) processing
    @ingroup	nl_tcd_modules
*******************************************************************************/

#include <stdint.h>

#ifndef NL_EHC_ADC_H_
#define NL_EHC_ADC_H_

#define ADC_CHANNELS (8)   // fixed !
#define SBUF_SIZE    (64)  // must be 2^N and larger than 3 (to hold the IIR buffer)

#define ADC_PIN_TO_5V     (0b10000000)  // not used atm
#define ADC_PIN_TO_PULLUP (0b01000000)  // tie input via 10kOhm to +5V

typedef struct
{
  uint16_t ipcAdcID;      // ADC channel value access ID
  uint16_t ipcDetectID;   // Plug detect access ID
  uint16_t ipcControlID;  // channel pullup control access ID
  struct
  {
    // input
    unsigned pullup_10k : 1;
    unsigned pullup_5V : 1;
    unsigned useIIR : 1;
    unsigned useStats : 1;
    // output
    unsigned initialized : 1;
  } flags;

  // raw data
  uint16_t values[SBUF_SIZE];             // raw values
  uint16_t filtered_values[SBUF_SIZE];    // values after IIR filtering
  uint8_t  detect_and_config[SBUF_SIZE];  // buffer for the cable detect(bit 0) and 5v/pullup config (bits 7/6)
  // pre-processed data
  uint16_t current;           // == filtered_values[sbuf_index]
  uint16_t filtered_current;  // == values[sbuf_index]
  uint8_t  detect;            // == detect_and_config[sbuf_index] & 1
} AdcBuffer_T;

extern AdcBuffer_T adc[ADC_CHANNELS];

// returns actually used "bufferDepth" (which is != 0) on success
int GetADCStats(AdcBuffer_T *this, int bufferDepth, uint16_t *min, uint16_t *max, uint16_t *avg);

void initSampleBuffers(void);
int  FillSampleBuffers(void);

#endif
//EOF
