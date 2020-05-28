#pragma once

#include <stddef.h>
#include <stdint.h>
#include "lpc-defs.h"

#ifdef __cplusplus
extern "C"
{
#endif

  enum EHC_PRESET_ID
  {
    EHC_PRESET_NONE = 0,
    EHC_PRESET_Pot_Tip_Active,
    EHC_PRESET_Pot_Ring_Active,
    EHC_PRESET_Pot_Tip_Act_Rev,
    EHC_PRESET_Pot_Ring_Act_Rev,
    EHC_PRESET_Resistor,
    EHC_PRESET_Resistor_Rev,
    EHC_PRESET_Switch_Closing,
    EHC_PRESET_Switch_Opening,
    EHC_PRESET_CV_0_5V,
    EHC_PRESET_CV_0_5V_Auto_Range,
    EHC_PRESET_OFF,
    EHC_PRESET_Boss_EV_30,
    EHC_PRESET_Boss_FV_500L,
    EHC_PRESET_Doepfer_FP5,
    EHC_PRESET_Fractal_EV_2,
    EHC_PRESET_Korg_DS_1H,
    EHC_PRESET_Korg_EXP_2,
    EHC_PRESET_Lead_Foot_LFX_1,
    EHC_PRESET_M_Audio_EX_P_M,
    EHC_PRESET_Moog_EP_3_std,
    EHC_PRESET_Roland_DP_10_cont,
    EHC_PRESET_Roland_EV_5,
    EHC_PRESET_Yamaha_FC3A,
    EHC_PRESET_Yamaha_FC7,
  };

  // ===================
  typedef struct
  {
    const enum EHC_PRESET_ID     id;
    const char *const            name;
    const char *const            displayName;
    const char *const            help;
    const EHC_ControllerConfig_T config;         // .ctrlId = 1 means "ring active"
    const int                    rangeMin;       // -1 means do not set
    const int                    rangeMax;       // -1 means do not set
    const int8_t                 deadZoneLower;  // -1 means do not set
    const int8_t                 deadZoneUpper;  // -1 means do not set
  } EHC_PresetT;

  extern const EHC_PresetT EHC_presets[];
  extern const unsigned    EHC_NUMBER_OF_PRESETS;

  EHC_PresetT const *EHC_GetPresetByName(char const *const name);
  EHC_PresetT const *EHC_GetPresetByDisplayName(char const *const displayName);
  EHC_PresetT const *EHC_GetPresetById(enum EHC_PRESET_ID const id);
  size_t             EHC_GetLPCMessageLength(void);

  enum EHC_RESET_T
  {
    EHC_NORESET = 0,
    EHC_RESET,
  };

  // returns the number of bytes put in the buffer, ==0 if error
  // port   : number of the TRS jack (1..4)
  // reset  : flags if the controller will be force-reset
  // buffer : to put composed LPC message in, set up the buffer length via EHC_GetLPCMessageLength()
  size_t EHC_ComposeLPCSetupMessageById(enum EHC_PRESET_ID const id,
                                        uint8_t const            port,
                                        enum EHC_RESET_T         reset,
                                        void *const              buffer);
  size_t EHC_ComposeLPCSetupMessageByPreset(EHC_PresetT const *const pPreset,
                                            uint8_t const            port,
                                            enum EHC_RESET_T         reset,
                                            void *const              buffer);

#ifdef __cplusplus
}
#endif
