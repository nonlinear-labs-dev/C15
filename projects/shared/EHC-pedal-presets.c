#include "EHC-pedal-presets.h"
#include "lpc-converters.h"
#include <string.h>

#define EHC_ADC_MAX (65520ul)

const EHC_PresetT EHC_presets[] = {
  {
      .id          = EHC_PRESET_Pot_Tip_Active,
      .name        = "pot-tip-active",
      .displayName = "Pot, Tip-Active",
      .help        = "Generic 3-Wire Expression/Damper Pedal, Tip-Active.\n"
              "If there is a \"min\" control, set it fully counter-clockwise.\n"
              "If there is a \"range\" control, set it fully clockwise.\n"
              "If you find the value progression isn't linear vs. pedal travel, try Ring-Active.",
      .config = {
          .ctrlId           = 0,
          .is3wire          = 1,
          .pullup           = 0,
          .continuous       = 1,
          .polarityInvert   = 0,
          .autoHoldStrength = 2,
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 5,  // 5% dead zone
      .deadZoneUpper = 5,  // 5% dead zone
  },
  {
      .id          = EHC_PRESET_Pot_Ring_Active,
      .name        = "pot-ring-active",
      .displayName = "Pot, Ring-Active",
      .help        = "Generic 3-Wire Expression/Damper Pedal, Ring-Active.\n"
              "If there is a \"min\" control, set it fully counter-clockwise.\n"
              "If there is a \"range\" control, set it fully clockwise.\n"
              "If you find the value progression isn't linear vs. pedal travel, try Tip-Active.",
      .config = {
          .ctrlId           = 1,  // ring active
          .is3wire          = 1,
          .pullup           = 0,
          .continuous       = 1,
          .polarityInvert   = 0,
          .autoHoldStrength = 2,
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 5,  // 5% dead zone
      .deadZoneUpper = 5,  // 5% dead zone
  },
  {
      .id          = EHC_PRESET_Pot_Tip_Act_Rev,
      .name        = "pot-tip-act-rev",
      .displayName = "Pot, Tip-Act. Rev.",  // reverse action
      .help        = "Generic 3-Wire Expression/Damper Pedal, Tip-Active, Reverse Action.\n"
              "If there is a \"min\" control, set it fully counter-clockwise.\n"
              "If there is a \"range\" control, set it fully clockwise.\n"
              "If you find the value progression not linear vs pedal travel, try Ring-Active.",
      .config = {
          .ctrlId           = 0,
          .is3wire          = 1,
          .pullup           = 0,
          .continuous       = 1,
          .polarityInvert   = 1,  // reverse action
          .autoHoldStrength = 2,
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 5,  // 5% dead zone
      .deadZoneUpper = 5,  // 5% dead zone
  },
  {
      .id          = EHC_PRESET_Pot_Ring_Act_Rev,
      .name        = "pot-ring-act-rev",
      .displayName = "Pot, Ring-Act. Rev.",  // reverse action
      .help        = "Generic 3-Wire Expression/Damper Pedal, Ring-Active, Reverse Action.\n"
              "If there is a \"min\" control, set it fully counter-clockwise.\n"
              "If there is a \"range\" control, set it fully clockwise.\n"
              "If you find the value progression not linear vs pedal travel, try Tip-Active.",
      .config = {
          .ctrlId           = 1,  // ring active
          .is3wire          = 1,
          .pullup           = 0,
          .continuous       = 1,
          .polarityInvert   = 1,  // reverse action
          .autoHoldStrength = 2,
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 5,  // 5% dead zone
      .deadZoneUpper = 5,  // 5% dead zone
  },
  {
      .id          = EHC_PRESET_Resistor,
      .name        = "resistor",
      .displayName = "Resistor",
      .help        = "Generic 2-Wire Adjustable Resistor, Tip-Active.",
      .config      = {
          .ctrlId           = 0,
          .is3wire          = 0,
          .pullup           = 1,
          .continuous       = 1,
          .polarityInvert   = 0,
          .autoHoldStrength = 2,
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = -1,
      .deadZoneUpper = -1,
  },
  {
      .id          = EHC_PRESET_Resistor_Rev,
      .name        = "resistor-rev",
      .displayName = "Resistor, Rev.",  // reverse action
      .help        = "Generic 2-Wire Adjustable Resistor, Tip-Active, Reverse Action.",
      .config      = {
          .ctrlId           = 0,
          .is3wire          = 0,
          .pullup           = 1,
          .continuous       = 1,
          .polarityInvert   = 1,
          .autoHoldStrength = 2,
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 5,  // 5% dead zone
      .deadZoneUpper = 5,  // 5% dead zone
  },
  {
      .id          = EHC_PRESET_Switch_Closing,
      .name        = "switch-closing",
      .displayName = "Switch, Closing",
      .help        = "Generic 2-Wire Momentary Switch, Tip-Active.\n"
              "Note: You can use most tip-active expression and damper pedals as switches, too.",
      .config = {
          .ctrlId           = 0,
          .is3wire          = 0,
          .pullup           = 1,
          .continuous       = 0,  // bi-stable
          .polarityInvert   = 0,
          .autoHoldStrength = 0,
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 1,  // 1% dead zone
      .deadZoneUpper = 1,  // 1% dead zone
  },
  {
      .id          = EHC_PRESET_Switch_Opening,
      .name        = "switch-opening",
      .displayName = "Switch, Opening",  // reverse action
      .help        = "Generic 2-Wire Momentary Switch, Tip-Active, Reverse Action.\n"
              "Note: You can use most tip-active expression and damper pedals as switches, too.",
      .config = {
          .ctrlId           = 0,
          .is3wire          = 0,
          .pullup           = 1,
          .continuous       = 0,  // bi-stable
          .polarityInvert   = 1,  // reverse action
          .autoHoldStrength = 0,
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 1,  // 1% dead zone
      .deadZoneUpper = 1,  // 1% dead zone
  },
  {
      .id          = EHC_PRESET_CV_0_5V,
      .name        = "cv-0-5v",
      .displayName = "CV, 0..5V",
      .help        = "Control Voltage, Tip-Active, Fixed Range: 0V..5V\n"
              "Do not apply voltages outside a 0V..5V range.\n"
              "Do not apply ANY voltage when the unit is off.",
      .config = {
          .ctrlId           = 0,
          .is3wire          = 0,  // 2-wire ...
          .pullup           = 0,  // ... and no pullup (unloaded input)
          .continuous       = 1,
          .polarityInvert   = 0,
          .autoHoldStrength = 1,  // assumed high value stability, for high resolution
          .doAutoRanging    = 0,  // fixed range
      },
      .rangeMin      = 10 * EHC_ADC_MAX / 5000,                   // 0V + 10mV
      .rangeMax      = EHC_ADC_MAX - (100 * EHC_ADC_MAX / 5000),  // 5V - 100mV
      .deadZoneLower = -1,                                        // not used with fixed range
      .deadZoneUpper = -1,                                        // not used with fixed range
  },
  {
      .id          = EHC_PRESET_CV_0_5V_Auto_Range,
      .name        = "cv-0-5v-auto-range",
      .displayName = "CV, 0..5V(Auto-Range)",
      .help        = "Control Voltage, Tip-Active, Auto-Ranging.\n"
              "Do not apply voltages outside a 0V..5V range.\n"
              "Do not apply ANY voltage when the unit is off.",
      .config = {
          .ctrlId           = 0,
          .is3wire          = 0,  // 2-wire ...
          .pullup           = 0,  // ... and no pullup (unloaded input)
          .continuous       = 1,
          .polarityInvert   = 0,
          .autoHoldStrength = 1,  // assumed high value stability, for high resolution
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 1,  // 1% dead zone
      .deadZoneUpper = 1,  // 1% dead zone
  },
  {
      .id          = EHC_PRESET_OFF,
      .name        = "off",
      .displayName = "---OFF---",
      .help        = "Ignore pedal even when connected.",
      .config      = {
          .hwId = 15,  // special, mark controller as "to be switched off"
      },
  },
  {
      .id          = EHC_PRESET_Boss_EV_30,
      .name        = "boss-ev-30",
      .displayName = "E:Boss EV-30",
      .help        = "Boss EV-30 3-Wire Expression Pedal, Tip-Active.\n"
              "Set \"min\" control to fully counter-clockwise.\n"
              "For reverse action, use output \"EXP2\" set to \"INV\"",
      .config = {
          .ctrlId           = 0,
          .is3wire          = 1,
          .pullup           = 0,
          .continuous       = 1,
          .polarityInvert   = 0,
          .autoHoldStrength = 2,
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 1,  // 1% dead zone
      .deadZoneUpper = 2,  // 2% dead zone
  },
  {
      .id          = EHC_PRESET_Boss_FV_500L,
      .name        = "boss-fv-500l",
      .displayName = "E:Boss FV-500L",
      .help        = "Boss FV-500L 3-Wire Expression Pedal, Tip-Active.\n"
              "Set \"min\" control to fully counter-clockwise.\n"
              "Use \"EXP\" output.",
      .config = {
          .ctrlId           = 0,
          .is3wire          = 1,
          .pullup           = 0,
          .continuous       = 1,
          .polarityInvert   = 0,
          .autoHoldStrength = 3,  // pretty unstable pedal
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 1,  // 1% dead zone
      .deadZoneUpper = 2,  // 2% dead zone
  },
  {
      .id          = EHC_PRESET_Doepfer_FP5,
      .name        = "doepfer-fp5",
      .displayName = "E:Doepfer FP5",
      .help        = "Doepfer FP5 3-Wire Expression Pedal, Ring-Active.",
      .config      = {
          .ctrlId           = 1,  // ring active
          .is3wire          = 1,
          .pullup           = 0,
          .continuous       = 1,
          .polarityInvert   = 0,
          .autoHoldStrength = 2,
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 1,  // 1% dead zone
      .deadZoneUpper = 4,  // 4% dead zone
  },
  {
      .id          = EHC_PRESET_Fractal_EV_2,
      .name        = "fractal-ev-2",
      .displayName = "E:Fractal EV-2",
      .help        = "Fractal EV-2 3-Wire Expression Pedal, Tip-Active.\n"
              "Use \"EXP\" output.",
      .config = {
          .ctrlId           = 0,
          .is3wire          = 1,
          .pullup           = 0,
          .continuous       = 1,
          .polarityInvert   = 0,
          .autoHoldStrength = 1,  // this is a very stable pedal
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 1,  // 1% dead zone, due to precision mechanics
      .deadZoneUpper = 1,  // 1% dead zone, due to precision mechanics
  },
  {
      .id          = EHC_PRESET_Korg_DS_1H,
      .name        = "korg-ds-1h",
      .displayName = "D:Korg DS-1H",
      .help        = "Korg DS-1H 2-Wire (Resistor) Damper Pedal, Tip-Active, Reverse Action.",
      .config      = {
          .ctrlId           = 0,
          .is3wire          = 0,
          .pullup           = 1,
          .continuous       = 1,
          .polarityInvert   = 1,  // reverse action
          .autoHoldStrength = 1,  // spring loaded return to zero, no massive hold required
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 3,  // 3% dead zone
      .deadZoneUpper = 7,  // 7% dead zone, end position has a lot of uncontrolled slack
  },
  {
      .id          = EHC_PRESET_Korg_EXP_2,
      .name        = "korg-exp-2",
      .displayName = "E:Korg EXP-2",
      .help        = "Korg EXP-2 3-Wire Expression Pedal, Ring-Active.\n"
              "Use output \"2\".",
      .config = {
          .ctrlId           = 1,  // ring active
          .is3wire          = 1,
          .pullup           = 0,
          .continuous       = 1,
          .polarityInvert   = 0,
          .autoHoldStrength = 2,
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 6,  // 6% dead zone
      .deadZoneUpper = 2,  // 2% dead zone
  },
  {
      .id          = EHC_PRESET_Lead_Foot_LFX_1,
      .name        = "lead-foot-lfx-1",
      .displayName = "E:Lead Foot LFX-1",
      .help        = "Lead Foot LFX-1 3-Wire Expression Pedal, Tip-Active.",
      .config      = {
          .ctrlId           = 0,
          .is3wire          = 1,
          .pullup           = 0,
          .continuous       = 1,
          .polarityInvert   = 0,
          .autoHoldStrength = 1,
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 1,  // 1% dead zone
      .deadZoneUpper = 1,  // 1% dead zone
  },
  {
      .id          = EHC_PRESET_M_Audio_EX_P_M,
      .name        = "m-audio-ex-p-m",
      .displayName = "E:M-Audio EX-P (M.)",
      .help        = "M-Audio EX-P 3-Wire Expression Pedal, Tip-Active.\n"
              "Set \"min\" control to fully counter-clockwise.\n"
              "Set bottom switch to \"M-Audio\".",
      .config = {
          .ctrlId           = 0,
          .is3wire          = 1,
          .pullup           = 0,
          .continuous       = 1,
          .polarityInvert   = 0,
          .autoHoldStrength = 1,  // very stable pedal
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 1,  // 1% dead zone, good mechanics
      .deadZoneUpper = 1,  // 1% dead zone, good mechanics
  },
  {
      .id          = EHC_PRESET_Moog_EP_3_std,
      .name        = "moog-ep-3-std",
      .displayName = "E:Moog EP-3 (std.)",
      .help        = "Moog EP-3 3-Wire Expression Pedal, Tip-Active.\n"
              "Set \"range\" control to fully clockwise.\n"
              "Set bottom switch to \"standard\"",
      .config = {
          .ctrlId           = 0,
          .is3wire          = 1,
          .pullup           = 0,
          .continuous       = 1,
          .polarityInvert   = 0,
          .autoHoldStrength = 3,  // rather unstable
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 1,  // 1% dead zone
      .deadZoneUpper = 8,  // 8% dead zone, upper end very bouncy
  },
  {
      .id          = EHC_PRESET_Roland_DP_10_cont,
      .name        = "roland-dp-10-cont",
      .displayName = "D:Roland DP-10 (cont.)",
      .help        = "Roland DP-10 3-Wire Damper Pedal, Tip-Active.\n"
              "Set switch to \"continuous\".",
      .config = {
          .ctrlId           = 0,
          .is3wire          = 1,
          .pullup           = 0,
          .continuous       = 1,
          .polarityInvert   = 0,
          .autoHoldStrength = 1,  // spring loaded return to zero, no massive hold required
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 7,  // 7% dead zone, zero position has a lot of uncontrolled slack
      .deadZoneUpper = 1,  // 1% dead zone
  },
  {
      .id          = EHC_PRESET_Roland_EV_5,
      .name        = "roland-ev-5",
      .displayName = "E:Roland EV-5",
      .help        = "Roland EV-5 3-Wire Expression Pedal, Tip-Active.\n"
              "Set \"min\" control to fully counter-clockwise.",
      .config = {
          .ctrlId           = 0,
          .is3wire          = 1,
          .pullup           = 0,
          .continuous       = 1,
          .polarityInvert   = 0,
          .autoHoldStrength = 1,  // rather stable pedal, high friction
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 1,  // 1% dead zone
      .deadZoneUpper = 2,  // 2% dead zone
  },
  {
      .id          = EHC_PRESET_Yamaha_FC3A,
      .name        = "yamaha-fc3a",
      .displayName = "D:Yamaha FC3A",
      .help        = "Yamaha FC3A 3-Wire Damper Pedal, Ring-Active, Reverse Action.",
      .config      = {
          .ctrlId           = 1,  // ring active
          .is3wire          = 1,
          .pullup           = 0,
          .continuous       = 1,
          .polarityInvert   = 1,  // reverse action
          .autoHoldStrength = 1,  // spring loaded return to zero, no massive hold required
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 4,  // 4% dead zone
      .deadZoneUpper = 1,  // 1% dead zone
  },
  {
      .id          = EHC_PRESET_Yamaha_FC7,
      .name        = "yamaha-fc7",
      .displayName = "E:Yamaha FC7",
      .help        = "Yamaha FC7 3-Wire Expression Pedal, Ring-Active.",
      .config      = {
          .ctrlId           = 1,  // ring active
          .is3wire          = 1,
          .pullup           = 0,
          .continuous       = 1,
          .polarityInvert   = 0,
          .autoHoldStrength = 3,  // pretty unstable pedal
          .doAutoRanging    = 1,
      },
      .rangeMin      = -1,
      .rangeMax      = -1,
      .deadZoneLower = 1,  // 1% dead zone
      .deadZoneUpper = 2,  // 2% dead zone
  },
  {
      .id = EHC_PRESET_NONE,  // END OF LIST marker
  },
};

const unsigned EHC_NUMBER_OF_PRESETS = (sizeof EHC_presets / sizeof EHC_presets[0]) - 1;  // ignore end-of-list marker preset

EHC_PresetT const *EHC_GetPresetByName(char const *const name)
{
  if (!name)
    return NULL;
  for (unsigned i = 0; EHC_presets[i].id != EHC_PRESET_NONE; i++)
  {
    if (strlen(name) == strlen(EHC_presets[i].name) && strcmp(name, EHC_presets[i].name) == 0)
      return &(EHC_presets[i]);
  }
  return NULL;
}

EHC_PresetT const *EHC_GetPresetByDisplayName(char const *const displayName)
{
  if (!displayName)
    return NULL;
  for (unsigned i = 0; EHC_presets[i].id != EHC_PRESET_NONE; i++)
  {
    if (strlen(displayName) == strlen(EHC_presets[i].displayName) && strcmp(displayName, EHC_presets[i].displayName) == 0)
      return &(EHC_presets[i]);
  }
  return NULL;
}

EHC_PresetT const *EHC_GetPresetById(enum EHC_PRESET_ID const id)
{
  if (id == EHC_PRESET_NONE)
    return NULL;
  for (unsigned i = 0; EHC_presets[i].id != EHC_PRESET_NONE; i++)
  {
    if (EHC_presets[i].id == id)
      return &(EHC_presets[i]);
  }
  return NULL;
}

size_t EHC_GetLPCMessageLength(void)
{
  return (4 * 2) * 2 + (4 * 2) * 6;
  // 2x enable/disable messages with 4 words (8 bytes)
  // + 6x config messages with 4 words (8 bytes)
  // --> 64 bytes total
}

static size_t writeData(uint8_t **const pBuf, size_t const len, uint16_t *data)
{
  for (size_t i = 0; i < len / 2; i++)
  {
    *(*pBuf)++ = *data & 0xFF;
    *(*pBuf)++ = *data++ >> 8;
  }
  return len;
}

size_t EHC_ComposeLPCSetupMessageById(enum EHC_PRESET_ID const id, uint8_t const port, enum EHC_RESET_T reset, void *const buffer)
{
  EHC_PresetT const *const pPreset = EHC_GetPresetById(id);
  if (!pPreset)
    return 0;
  return EHC_ComposeLPCSetupMessageByPreset(pPreset, port, reset, buffer);
}

size_t EHC_ComposeLPCSetupMessageByPreset(EHC_PresetT const *const pPreset, uint8_t const port, enum EHC_RESET_T reset, void *const buffer)
{
  if (port < 1 || port > 4 || !pPreset || !buffer)
    return 0;

  uint16_t CMD_DATA[]   = { LPC_BB_MSG_TYPE_EHC_CONFIG, 0x0002, 0x0000, 0x0000 };
  uint16_t ENDIS_DATA[] = { LPC_BB_MSG_TYPE_SETTING, 0x0002, LPC_SETTING_ID_ENABLE_EHC, 0x0000 };

  uint8_t *bufp         = (uint8_t *) buffer;
  size_t   bytesWritten = 0;

  // disable EHC processing
  ENDIS_DATA[3] = 0;
  bytesWritten += writeData(&bufp, sizeof ENDIS_DATA, &ENDIS_DATA[0]);

  EHC_ControllerConfig_T config = pPreset->config;
  if (config.hwId != 15)     // normal config?
    config.hwId = port - 1;  // set HWSID corresponding to port number
  else
    reset = EHC_RESET;  // force reset for "off" type controller

  config.ctrlId += (port - 1) * 2;  // set CTRLID  corresponding to port number, factoring in the tip/ring pre-selection

  // clear adjacent controller
  EHC_ControllerConfig_T clearConfig = config;
  clearConfig.ctrlId ^= 1;  // select adjacent controller
  clearConfig.hwId = 15;    // special ID to clear controller
  CMD_DATA[2]      = LPC_EHC_COMMAND_SET_CONTROL_REGISTER;
  CMD_DATA[3]      = EHC_configToUint16(clearConfig);
  bytesWritten += writeData(&bufp, sizeof CMD_DATA, &CMD_DATA[0]);

  // clear this controller if forced reset
  if (reset)
  {
    clearConfig.ctrlId = config.ctrlId;  // select this controller
    CMD_DATA[3]        = EHC_configToUint16(clearConfig);
    bytesWritten += writeData(&bufp, sizeof CMD_DATA, &CMD_DATA[0]);
  }

  // send main configuration, unless it was "off" type controller
  if (config.hwId != 15)
  {
    CMD_DATA[3] = EHC_configToUint16(config);
    bytesWritten += writeData(&bufp, sizeof CMD_DATA, &CMD_DATA[0]);

    // send range information, if any
    if (pPreset->rangeMin >= 0 && pPreset->rangeMax >= 0)
    {
      CMD_DATA[2] = LPC_EHC_COMMAND_SET_RANGE_MIN | config.ctrlId;
      CMD_DATA[3] = pPreset->rangeMin;
      bytesWritten += writeData(&bufp, sizeof CMD_DATA, &CMD_DATA[0]);

      CMD_DATA[2] = LPC_EHC_COMMAND_SET_RANGE_MAX | config.ctrlId;
      CMD_DATA[3] = pPreset->rangeMax;
      bytesWritten += writeData(&bufp, sizeof CMD_DATA, &CMD_DATA[0]);
    }

    // send deadzone information, if any
    if (pPreset->deadZoneLower >= 0 && pPreset->deadZoneUpper >= 0)
    {
      CMD_DATA[2] = LPC_EHC_COMMAND_SET_DEAD_ZONES | config.ctrlId;
      CMD_DATA[3] = ((uint16_t) pPreset->deadZoneUpper << 8) + pPreset->deadZoneLower;
      bytesWritten += writeData(&bufp, sizeof CMD_DATA, &CMD_DATA[0]);
    }
  }

  // enable EHC processing
  ENDIS_DATA[3] = 1;
  bytesWritten += writeData(&bufp, sizeof ENDIS_DATA, &ENDIS_DATA[0]);

  return bytesWritten;
}
