#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#pragma GCC diagnostic ignored "-Wunused-function"

#include "shared/lpc-defs.h"
#include "shared/lpc-converters.h"
#include "shared/EHC-pedal-presets.h"

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
  puts("  preset-name : name of the preset, or its display name, enclosed in \"\n");
  puts("  port        : port (TRS jack) number 1..4");
  puts("  reset       : forces a complete re-init of the pedal (reset auto-ranging)");
  exit(3);
}

// ===================
#define LIST    "list"
#define DETAILS "details"
#define RESET   "reset"

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
      printf("%s\n", EHC_presets[i].name);
    return 0;
  }

  // details
  if (strncmp(argv[1], DETAILS, sizeof DETAILS) == 0)
  {
    for (unsigned i = 0; i < EHC_NUMBER_OF_PRESETS; i++)
    {
      printf("name = %s\n", EHC_presets[i].name);
      printf("display name = \'%s\'\n", EHC_presets[i].displayName);
      if (EHC_presets[i].help)
        printf("help text =\n\'%s\'\n\n", EHC_presets[i].help);
    }
    return 0;
  }

  if (argc == 2)
    Usage();

  // search pedal name and apply settings when found
  EHC_PresetT const *pPreset = EHC_GetPresetByName(argv[1]);
  if (!pPreset)
    pPreset = EHC_GetPresetByDisplayName(argv[1]);
  if (!pPreset)
  {
    puts("Preset not found!");
    Usage();
  }

  uint16_t         buffer[EHC_GetLPCMessageLength() / 2];
  uint16_t         port  = readPortNumber(argv[2]);
  enum EHC_RESET_T reset = EHC_NORESET;
  if (argc > 3)
  {
    if (strncmp(argv[3], RESET, sizeof RESET) == 0)
      reset = EHC_RESET;
    else
      Usage();
  }

  size_t bytesToSend = EHC_ComposeLPCSetupMessageByPreset(pPreset, port, reset, buffer);
  if (bytesToSend)
  {
    writeData(driver, bytesToSend, buffer);
    return 0;
  }

  puts("Internal Error");
  Usage();
}
