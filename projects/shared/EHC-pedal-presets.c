#include "EHC-pedal-presets.h"

#define EHC_ADC_MAX (65520ul)

const EHC_PresetT EHC_presets[] = {
  {
      .name = "Pot, Tip-Active",
      .help = "Generic 3-Wire Expression/Damper Pedal, Tip-Active.\n"
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
      .name = "Pot, Ring-Active",
      .help = "Generic 3-Wire Expression/Damper Pedal, Ring-Active.\n"
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
      .name = "Pot, Tip-Act. Rev.",  // reverse action
      .help = "Generic 3-Wire Expression/Damper Pedal, Tip-Active, Reverse Action.\n"
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
      .name = "Pot, Ring-Act. Rev.",  // reverse action
      .help = "Generic 3-Wire Expression/Damper Pedal, Ring-Active, Reverse Action.\n"
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
      .name   = "Resistor",
      .help   = "Generic 2-Wire Adjustable Resistor, Tip-Active.",
      .config = {
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
      .name   = "Resistor, Rev.",  // reverse action
      .help   = "Generic 2-Wire Adjustable Resistor, Tip-Active, Reverse Action.",
      .config = {
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
      .name = "Switch, Closing",
      .help = "Generic 2-Wire Momentary Switch, Tip-Active.\n"
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
      .name = "Switch, Opening",  // reverse action
      .help = "Generic 2-Wire Momentary Switch, Tip-Active, Reverse Action.\n"
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
      .name = "CV, 0..5V",
      .help = "Control Voltage, Tip-Active, Fixed Range: 0V..5V\n"
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
      .name = "CV, 0..5V(Auto-Range)",
      .help = "Control Voltage, Tip-Active, Auto-Ranging.\n"
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
      .name   = "---OFF---",
      .help   = "Ignore pedal even when connected.",
      .config = {
          .hwId = 15,  // special, mark controller as "to be switched off"
      },
  },
  {
      .name = "E:Boss EV-30",
      .help = "Boss EV-30 3-Wire Expression Pedal, Tip-Active.\n"
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
      .name = "E:Boss FV-500L",
      .help = "Boss FV-500L 3-Wire Expression Pedal, Tip-Active.\n"
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
      .name   = "E:Doepfer FP5",
      .help   = "Doepfer FP5 3-Wire Expression Pedal, Ring-Active.",
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
      .deadZoneLower = 1,  // 1% dead zone
      .deadZoneUpper = 4,  // 4% dead zone
  },
  {
      .name = "E:Fractal EV-2",
      .help = "Fractal EV-2 3-Wire Expression Pedal, Tip-Active.\n"
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
      .name   = "D:Korg DS-1H",
      .help   = "Korg DS-1H 2-Wire (Resistor) Damper Pedal, Tip-Active, Reverse Action.",
      .config = {
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
      .name = "E:Korg EXP-2",
      .help = "Korg EXP-2 3-Wire Expression Pedal, Ring-Active.\n"
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
      .name   = "E:Lead Foot LFX-1",
      .help   = "Lead Foot LFX-1 3-Wire Expression Pedal, Tip-Active.",
      .config = {
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
      .name = "E:M-Audio EX-P (M.)",
      .help = "M-Audio EX-P 3-Wire Expression Pedal, Tip-Active.\n"
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
      .name = "E:Moog EP-3 (std.)",
      .help = "Moog EP-3 3-Wire Expression Pedal, Tip-Active.\n"
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
      .name = "D:Roland DP-10 (cont.)",
      .help = "Roland DP-10 3-Wire Damper Pedal, Tip-Active.\n"
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
      .name = "E:Roland EV-5",
      .help = "Roland EV-5 3-Wire Expression Pedal, Tip-Active.\n"
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
      .name   = "D:Yamaha FC3A",
      .help   = "Yamaha FC3A 3-Wire Damper Pedal, Ring-Active, Reverse Action.",
      .config = {
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
      .name   = "E:Yamaha FC7",
      .help   = "Yamaha FC7 3-Wire Expression Pedal, Ring-Active.",
      .config = {
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
};

const unsigned EHC_NUMBER_OF_PRESETS = (sizeof EHC_presets / sizeof EHC_presets[0]);
