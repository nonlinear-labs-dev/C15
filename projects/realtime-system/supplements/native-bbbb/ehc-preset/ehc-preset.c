#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#pragma GCC diagnostic ignored "-Wunused-function"

#include "../../../../shared/lpc-defs.h"
#include "../../../../shared/lpc-converters.h"

// ===================
typedef struct
{
  const char *const            name;
  const char *const            help;
  const EHC_ControllerConfig_T config;         // .ctrlId = 1 means "ring active"
  const int                    rangeMin;       // -1 means do not set
  const int                    rangeMax;       // -1 means do not set
  const int8_t                 deadZoneLower;  // -1 means do not set
  const int8_t                 deadZoneUpper;  // -1 means do not set
} EHC_PresetT;

#define ADC_MAX (65520ul)

const EHC_PresetT presets[] = {
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
      .rangeMin      = 10 * ADC_MAX / 5000,               // 0V + 10mV
      .rangeMax      = ADC_MAX - (100 * ADC_MAX / 5000),  // 5V - 100mV
      .deadZoneLower = -1,                                // not used with fixed range
      .deadZoneUpper = -1,                                // not used with fixed range
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
              "Use output \"2\"",
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
      .deadZoneLower = 6,  // 5% dead zone
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

#define NUMBER_OF_PRESETS (sizeof presets / sizeof presets[0])

// ===================
void IOerror(int ret)
{
  puts("\nI/O error (driver). Terminating.");
  exit(ret);
}

// ===================
void writeData(FILE *const output, uint16_t const len, uint16_t *data)
{
  if (len == 0)
    return;

  for (uint16_t i = 0; i < len / 2; i++)
  {
    if (fputc(*data & 0xFF, output) == EOF)
      IOerror(3);
    if (fputc((*data++ & 0xFF00) >> 8, output) == EOF)
      IOerror(3);
  }
}

// ===================
void Usage(void)
{
  puts("Usage:");
  puts(" ehc-preset  list|details : list available presets, with details");
  puts(" ehc-preset  <preset-name> <port> [reset]");
  puts("  preset-name : name of the preset, enclosed in \"\n");
  puts("  port        : port (TRS jack) number 1..4");
  puts("  reset       : forces a complete re-init of the pedal (reset auto-ranging)");
  exit(3);
}

// ===================
#define LIST    "list"
#define DETAILS "details"
#define RESET   "reset"

uint16_t CMD_DATA[]   = { LPC_BB_MSG_TYPE_EHC_CONFIG, 0x0002, 0x0000, 0x0000 };
uint16_t ENDIS_DATA[] = { LPC_BB_MSG_TYPE_SETTING, 0x0002, LPC_SETTING_ID_ENABLE_EHC, 0x0000 };

uint16_t readPortNumber(const char *string)
{
  uint16_t id;
  if (sscanf(string, "%hu", &id) != 1)
  {
    puts("config: argument error (controller id expected)");
    Usage();
  }
  if (id < 1 || id > 4)
  {
    puts("port number must be 1..4");
    Usage();
  }
  return id;
}

#define DRIVER "/dev/lpc_bb_driver"
//#define DRIVER "test.bin"
// ===================
int main(int argc, char const *argv[])
{
  FILE *driver;

  if (argc == 1)
    Usage();

  driver = fopen(DRIVER, "r+");
  if (!driver)
    IOerror(3);

  // list
  if (strncmp(argv[1], LIST, sizeof LIST) == 0)
  {
    for (unsigned i = 0; i < NUMBER_OF_PRESETS; i++)
      printf("\"%s\"\n", presets[i].name);
    return 0;
  }

  // details
  if (strncmp(argv[1], DETAILS, sizeof DETAILS) == 0)
  {
    for (unsigned i = 0; i < NUMBER_OF_PRESETS; i++)
    {
      printf("\"%s\"\n", presets[i].name);
      if (presets[i].help)
        printf("%s\n\n", presets[i].help);
    }
    return 0;
  }

  if (argc == 2)
    Usage();

  // search pedal name and apply settings when found
  for (unsigned presetIndex = 0; presetIndex < NUMBER_OF_PRESETS; presetIndex++)
  {
    if (strlen(argv[1]) == strlen(presets[presetIndex].name)
        && strcmp(argv[1], presets[presetIndex].name) == 0)
    {
      uint16_t port  = readPortNumber(argv[2]);
      int      reset = 0;
      if (argc > 3)
      {
        if (strncmp(argv[3], RESET, sizeof RESET) == 0)
          reset = 1;
        else
          Usage();
      }

      // disable EHC processing
      ENDIS_DATA[3] = 0;
      writeData(driver, sizeof ENDIS_DATA, &ENDIS_DATA[0]);

      EHC_ControllerConfig_T config        = presets[presetIndex].config;
      int                    rangeMin      = presets[presetIndex].rangeMin;
      int                    rangeMax      = presets[presetIndex].rangeMax;
      int8_t                 deadZoneLower = presets[presetIndex].deadZoneLower;
      int8_t                 deadZoneUpper = presets[presetIndex].deadZoneUpper;

      if (config.hwId != 15)     // normal config?
        config.hwId = port - 1;  // set HWSID corresponding to port number
      else
        reset = 1;  // force reset for "off" type controller

      config.ctrlId += (port - 1) * 2;  // set CTRLID  corresponding to port number, factoring in the tip/ring pre-selection

      // clear adjacent controller
      EHC_ControllerConfig_T clearConfig = config;
      clearConfig.ctrlId ^= 1;  // select adjacent controller
      clearConfig.hwId = 15;    // special ID to clear controller
      CMD_DATA[2]      = LPC_EHC_COMMAND_SET_CONTROL_REGISTER;
      CMD_DATA[3]      = EHC_configToUint16(clearConfig);
      writeData(driver, sizeof CMD_DATA, &CMD_DATA[0]);

      // clear this controller if forced reset
      if (reset)
      {
        clearConfig.ctrlId = config.ctrlId;  // select this controller
        CMD_DATA[3]        = EHC_configToUint16(clearConfig);
        writeData(driver, sizeof CMD_DATA, &CMD_DATA[0]);
      }

      // send main configuration, unless it was "off" type controller
      if (config.hwId != 15)
      {
        CMD_DATA[3] = EHC_configToUint16(config);
        writeData(driver, sizeof CMD_DATA, &CMD_DATA[0]);

        // send range information, if any
        if (rangeMin >= 0 && rangeMax >= 0)
        {
          CMD_DATA[2] = LPC_EHC_COMMAND_SET_RANGE_MIN | config.ctrlId;
          CMD_DATA[3] = rangeMin;
          writeData(driver, sizeof CMD_DATA, &CMD_DATA[0]);

          CMD_DATA[2] = LPC_EHC_COMMAND_SET_RANGE_MAX | config.ctrlId;
          CMD_DATA[3] = rangeMax;
          writeData(driver, sizeof CMD_DATA, &CMD_DATA[0]);
        }

        // send deadzone information, if any
        if (deadZoneLower >= 0 && deadZoneUpper >= 0)
        {
          CMD_DATA[2] = LPC_EHC_COMMAND_SET_DEAD_ZONES | config.ctrlId;
          CMD_DATA[3] = ((uint16_t) deadZoneUpper << 8) + deadZoneLower;
          writeData(driver, sizeof CMD_DATA, &CMD_DATA[0]);
        }
      }

      // enable EHC processing
      ENDIS_DATA[3] = 1;
      writeData(driver, sizeof ENDIS_DATA, &ENDIS_DATA[0]);

      return 0;
    }
  }
  puts("Preset not found!");
  Usage();
}
