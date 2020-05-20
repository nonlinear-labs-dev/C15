#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "shared/lpc-defs.h"

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

#define REQUEST         "req"
#define SW_VERSION      "sw-version"
#define MUTE_STATUS     "muting"
#define CLEAR_EEPROM    "clear-eeprom"
#define STAT_DATA       "status"
#define EHC_SAVE_EEPROM "save-ehc"

#define SETTING             "set"
#define MUTE_CTRL           "mute-ctrl"
#define MUTE_CTRL_DISABLE   "disable"
#define MUTE_CTRL_MUTE      "mute"
#define MUTE_CTRL_UNMUTE    "unmute"
#define RAW_SENSORS         "sensors"
#define KEY_LOGGING         "key-logging"
#define ON                  "on"
#define OFF                 "off"
#define AE_CMD              "ae-cmd"
#define AE_CMD_TTON         "tton"
#define AE_CMD_TTOFF        "ttoff"
#define AE_CMD_DEFSND       "def-snd"
#define SSPECIAL            "system"
#define SSPECIAL_RESET      "reboot"
#define SSPECIAL_RESETHB    "hb-reset"
#define SSPECIAL_ENABLEMIDI "enable-midi"

#define KEY_EMUL "key"

uint16_t REQ_DATA[] = { LPC_BB_MSG_TYPE_REQUEST, 0x0001, 0x0000 };
uint16_t SET_DATA[] = { LPC_BB_MSG_TYPE_SETTING, 0x0002, 0x0000, 0x0000 };
uint16_t KEY_DATA[] = { LPC_BB_MSG_TYPE_KEY_EMUL, 0x0003, 0x0000, 0x0000, 0x0000 };

// ===================
void Usage(void)
{
  puts("Usage:");
  puts(" lpc  <command>");
  puts("  <commands> : req|set|key");
  puts("  req[uest] : sw-version|muting|clear-eeprom|status|save-ehc");
  puts("     sw-version   : get LPC firware version");
  puts("     muting       : get software&hardware muting status");
  puts("     clear-eeprom : erase EEPROM");
  puts("     status       : get diagnostic status data (and clear it)");
  puts("     save-ehc     : save current EHC config data to EEPROM");
  puts("  set[ting] : mute-ctrl|sensors|key-logging|ae-cmd|system");
  puts("     mute-ctrl: disable|mute|unmute : disable mute override or set/clear muting");
  puts("     sensors: on|off                : turn raw sensor messages on/off");
  puts("     key-logging: on|off            : turn key-logging messages on/off");
  puts("     ae-cmd: tton|ttoff|def-snd     : Audio Engine Special, test-tone on/off, load default sound");
  puts("     system: reboot|hb-reset|enable-midi");
  puts("                  : System Special; reboot system, reset heartbeat counter, enable midi");
  puts("  key : <note-nr> <time>  : send emulated key");
  puts("     <note-nr>              : MIDI key number, 60=\"C3\"");
  puts("     <time>                 : key time (~1/velocity) in ms (2...525), negative means key release");
  exit(3);
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

  // request
  if (strncmp(argv[1], REQUEST, sizeof REQUEST) == 0)
  {
    if (argc != 3)
    {
      puts("req: too few arguments!");
      Usage();
    }
    if (strncmp(argv[2], SW_VERSION, sizeof SW_VERSION) == 0)
    {
      REQ_DATA[2] = LPC_REQUEST_ID_SW_VERSION;
      writeData(driver, sizeof REQ_DATA, &REQ_DATA[0]);
      return 0;
    }
    if (strncmp(argv[2], MUTE_STATUS, sizeof MUTE_STATUS) == 0)
    {
      REQ_DATA[2] = LPC_REQUEST_ID_UNMUTE_STATUS;
      writeData(driver, sizeof REQ_DATA, &REQ_DATA[0]);
      return 0;
    }
    if (strncmp(argv[2], CLEAR_EEPROM, sizeof CLEAR_EEPROM) == 0)
    {
      REQ_DATA[2] = LPC_REQUEST_ID_CLEAR_EEPROM;
      writeData(driver, sizeof REQ_DATA, &REQ_DATA[0]);
      return 0;
    }
    if (strncmp(argv[2], STAT_DATA, sizeof STAT_DATA) == 0)
    {
      REQ_DATA[2] = LPC_REQUEST_ID_STAT_DATA;
      writeData(driver, sizeof REQ_DATA, &REQ_DATA[0]);
      return 0;
    }
    if (strncmp(argv[2], EHC_SAVE_EEPROM, sizeof EHC_SAVE_EEPROM) == 0)
    {
      REQ_DATA[2] = LPC_REQUEST_ID_EHC_EEPROMSAVE;
      writeData(driver, sizeof REQ_DATA, &REQ_DATA[0]);
      return 0;
    }
    puts("req: unknown request");
    Usage();
  }

  // setting
  if (strncmp(argv[1], SETTING, sizeof SETTING) == 0)
  {
    if (argc != 4)
    {
      puts("set: too few arguments (need 2)!");
      Usage();
    }
    // mute control
    if (strncmp(argv[2], MUTE_CTRL, sizeof MUTE_CTRL) == 0)
    {
      SET_DATA[2] = LPC_SETTING_ID_SOFTWARE_MUTE_OVERRIDE;
      if (strncmp(argv[3], MUTE_CTRL_DISABLE, sizeof MUTE_CTRL_DISABLE) == 0)
      {
        SET_DATA[3] = 0;
        writeData(driver, sizeof SET_DATA, &SET_DATA[0]);
        return 0;
      }
      if (strncmp(argv[3], MUTE_CTRL_MUTE, sizeof MUTE_CTRL_MUTE) == 0)
      {
        SET_DATA[3] = SUP_UNMUTE_STATUS_SOFTWARE_OVERRIDE;
        writeData(driver, sizeof SET_DATA, &SET_DATA[0]);
        return 0;
      }
      if (strncmp(argv[3], MUTE_CTRL_UNMUTE, sizeof MUTE_CTRL_UNMUTE) == 0)
      {
        SET_DATA[3] = SUP_UNMUTE_STATUS_SOFTWARE_OVERRIDE | SUP_UNMUTE_STATUS_SOFTWARE_VALUE;
        writeData(driver, sizeof SET_DATA, &SET_DATA[0]);
        return 0;
      }
      puts("set mute-ctrl : illegal parameter");
      Usage();
    }

    // sensors
    if (strncmp(argv[2], RAW_SENSORS, sizeof RAW_SENSORS) == 0)
    {
      SET_DATA[2] = LPC_SETTING_ID_SEND_RAW_SENSOR_DATA;
      if (strncmp(argv[3], OFF, sizeof OFF) == 0)
      {
        SET_DATA[3] = 0;
        writeData(driver, sizeof SET_DATA, &SET_DATA[0]);
        return 0;
      }
      if (strncmp(argv[3], ON, sizeof ON) == 0)
      {
        SET_DATA[3] = 1;
        writeData(driver, sizeof SET_DATA, &SET_DATA[0]);
        return 0;
      }
      puts("set sensors : illegal parameter");
      Usage();
    }

    // key-logging
    if (strncmp(argv[2], KEY_LOGGING, sizeof KEY_LOGGING) == 0)
    {
      SET_DATA[2] = LPC_SETTING_ID_ENABLE_KEY_LOGGING;
      if (strncmp(argv[3], OFF, sizeof OFF) == 0)
      {
        SET_DATA[3] = 0;
        writeData(driver, sizeof SET_DATA, &SET_DATA[0]);
        return 0;
      }
      if (strncmp(argv[3], ON, sizeof ON) == 0)
      {
        SET_DATA[3] = 1;
        writeData(driver, sizeof SET_DATA, &SET_DATA[0]);
        return 0;
      }
      puts("set key-logging : illegal parameter");
      Usage();
    }

    // audio engine command
    if (strncmp(argv[2], AE_CMD, sizeof AE_CMD) == 0)
    {
      SET_DATA[2] = LPC_SETTING_ID_AUDIO_ENGINE_CMD;
      if (strncmp(argv[3], AE_CMD_TTOFF, sizeof AE_CMD_TTOFF) == 0)
      {
        SET_DATA[3] = AE_CMD_TONE_OFF;
        writeData(driver, sizeof SET_DATA, &SET_DATA[0]);
        return 0;
      }
      if (strncmp(argv[3], AE_CMD_TTON, sizeof AE_CMD_TTON) == 0)
      {
        SET_DATA[3] = AE_CMD_TONE_ON;
        writeData(driver, sizeof SET_DATA, &SET_DATA[0]);
        return 0;
      }
      if (strncmp(argv[3], AE_CMD_DEFSND, sizeof AE_CMD_DEFSND) == 0)
      {
        SET_DATA[3] = AE_CMD_DEFAULT_SOUND;
        writeData(driver, sizeof SET_DATA, &SET_DATA[0]);
        return 0;
      }
      puts("set ae-cmd : illegal parameter");
      Usage();
    }

    // system special
    if (strncmp(argv[2], SSPECIAL, sizeof SSPECIAL) == 0)
    {
      SET_DATA[2] = LPC_SETTING_ID_SYSTEM_SPECIAL;
      if (strncmp(argv[3], SSPECIAL_RESET, sizeof SSPECIAL_RESET) == 0)
      {
        SET_DATA[3] = SYS_SPECIAL_RESET_SYSTEM;
        writeData(driver, sizeof SET_DATA, &SET_DATA[0]);
        return 0;
      }
      if (strncmp(argv[3], SSPECIAL_RESETHB, sizeof SSPECIAL_RESETHB) == 0)
      {
        SET_DATA[3] = SYS_SPECIAL_RESET_HEARTBEAT;
        writeData(driver, sizeof SET_DATA, &SET_DATA[0]);
        return 0;
      }
      if (strncmp(argv[3], SSPECIAL_ENABLEMIDI, sizeof SSPECIAL_ENABLEMIDI) == 0)
      {
        SET_DATA[3] = SYS_SPECIAL_ENABLE_MIDI;
        writeData(driver, sizeof SET_DATA, &SET_DATA[0]);
        return 0;
      }
      puts("set system : illegal parameter");
      Usage();
    }
    puts("set: unknown parameter!");
    Usage();
  }

  // key emulation
  if (strncmp(argv[1], KEY_EMUL, sizeof KEY_EMUL) == 0)
  {
    if (argc != 4)
    {
      puts("set: too few arguments!");
      Usage();
    }
    if (sscanf(argv[2], "%hu", &KEY_DATA[2]) != 1)
    {
      puts("key: key number argument error (uint16 expected)");
      Usage();
    }
    int time;
    if (sscanf(argv[3], "%d", &time) != 1)
    {
      puts("key: key time argument error (int32 expected)");
      Usage();
    }
    KEY_DATA[3] = time & 0xFFFF;
    KEY_DATA[4] = (time >> 16) & 0xFFFF;
    writeData(driver, sizeof KEY_DATA, &KEY_DATA[0]);
    return 0;
  }

  // unknown
  puts("unknown command");
  Usage();
  return 0;
}
