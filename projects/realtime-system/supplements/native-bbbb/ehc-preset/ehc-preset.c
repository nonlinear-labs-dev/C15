#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#pragma GCC diagnostic ignored "-Wunused-function"

#include "lpc-defs.h"
#include "lpc-converters.h"
#include "EHC-pedal-presets.h"

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
    puts("argument error (port number expected)");
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
    for (unsigned i = 0; i < EHC_NUMBER_OF_PRESETS; i++)
      printf("\"%s\"\n", EHC_presets[i].name);
    return 0;
  }

  // details
  if (strncmp(argv[1], DETAILS, sizeof DETAILS) == 0)
  {
    for (unsigned i = 0; i < EHC_NUMBER_OF_PRESETS; i++)
    {
      printf("\"%s\"\n", EHC_presets[i].name);
      if (EHC_presets[i].help)
        printf("%s\n\n", EHC_presets[i].help);
    }
    return 0;
  }

  if (argc == 2)
    Usage();

  // search pedal name and apply settings when found
  for (unsigned presetIndex = 0; presetIndex < EHC_NUMBER_OF_PRESETS; presetIndex++)
  {
    if (strlen(argv[1]) == strlen(EHC_presets[presetIndex].name)
        && strcmp(argv[1], EHC_presets[presetIndex].name) == 0)
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

      EHC_ControllerConfig_T config        = EHC_presets[presetIndex].config;
      int                    rangeMin      = EHC_presets[presetIndex].rangeMin;
      int                    rangeMax      = EHC_presets[presetIndex].rangeMax;
      int8_t                 deadZoneLower = EHC_presets[presetIndex].deadZoneLower;
      int8_t                 deadZoneUpper = EHC_presets[presetIndex].deadZoneUpper;

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
