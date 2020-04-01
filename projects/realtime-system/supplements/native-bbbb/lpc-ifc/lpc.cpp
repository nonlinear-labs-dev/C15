#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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
#define BB_MSG_TYPE_PARAMETER    0x0400  // direction: output; arguments(uint16): 2, 1x parameter ID , 1x data
#define BB_MSG_TYPE_SETTING      0x0700  // direction: input;  arguments (uint16): 2, 1x SETTING_ID_*, 1x data
#define BB_MSG_TYPE_NOTIFICATION 0x0800  // direction: output; arguments(uint16): 2, 1x type, 1x value
#define BB_MSG_TYPE_ASSERTION    0x0900  // direction: output; arguments(uint16): n (string)
#define BB_MSG_TYPE_REQUEST      0x0A00  // direction: input;  argument (uint16): 1, 1x REQUEST_ID_*
#define BB_MSG_TYPE_HEARTBEAT    0x0B00  // direction: output; arguments(uint16): 4, 4x uint16 (==uint64)
#define BB_MSG_TYPE_MUTESTATUS   0x0C00  // direction: output; argument (uint16): 1, 1x bit pattern
#define BB_MSG_TYPE_RIBBON_CAL   0x0D00  // direction: input; arguments(uint16): 134, 134x data [2x (33x 34x)]
#define BB_MSG_TYPE_SENSORS_RAW  0x0E00  // direction: output; arguments(uint16): 13, sensor raw data (see nl_tcd_adc_work.c)
#define BB_MSG_TYPE_EHC_CONFIG   0x0F00  // direction: input;  arguments (uint16): 2, 1x command, 1x data
#define BB_MSG_TYPE_EHC_DATA     0x1000  // direction: output;  arguments(uint16): ??, (see nl_ehc_ctrl.c)
#define BB_MSG_TYPE_KEY_EMUL     0x1100  // direction: input;  arguments (uint16): 3, midi key , time(lo), time(high)
#define BB_MSG_TYPE_COOS_DATA    0x1200  // direction: output;  arguments (uint16): 4

//----- Setting Ids:

#define SETTING_ID_PLAY_MODE_UPPER_RIBBON_BEHAVIOUR 0  // ==> BIT 0 set if (returnMode == RETURN), ...
#define SETTING_ID_PLAY_MODE_LOWER_RIBBON_BEHAVIOUR 1  // ... BIT 1 set if (touchBehaviour == RELATIVE)

#define SETTING_ID_BASE_UNIT_UI_MODE 3  // ==> PLAY = 0, PARAMETER_EDIT = 1

#define SETTING_ID_EDIT_MODE_RIBBON_BEHAVIOUR 4  // ==> RELATIVE = 0, ABSOLUTE = 1

#define SETTING_ID_UPPER_RIBBON_REL_FACTOR 9   // ==> tTcdRange(256, 2560)
#define SETTING_ID_LOWER_RIBBON_REL_FACTOR 10  // ==> tTcdRange(256, 2560)

#define SETTING_ID_VELOCITY_CURVE 11  // ==> VERY_SOFT = 0, SOFT = 1, NORMAL = 2, HARD = 3, VERY_HARD = 4

// SETTING_ID_PEDAL_x_TYPE must be a monotonic rising sequence
#define SETTING_ID_PEDAL_1_TYPE 26  // ==> PotTipActive  = 0
#define SETTING_ID_PEDAL_2_TYPE 27  // ... PotRingActive = 1
#define SETTING_ID_PEDAL_3_TYPE 28  // ... SwitchClosing = 2 // aka momentary switch, normally open
#define SETTING_ID_PEDAL_4_TYPE 29  // ... SwitchOpening = 3 // aka momentary switch, normally closed

#define SETTING_ID_AFTERTOUCH_CURVE 30  // SOFT = 0, NORMAL = 1, HARD = 2
#define SETTING_ID_BENDER_CURVE     31  // SOFT = 0, NORMAL = 1, HARD = 2

// new setting ID's
#define SETTING_ID_SOFTWARE_MUTE_OVERRIDE 0xFF01  // direction: input; arguments(uint16): 1, mode bit pattern
#define SETTING_ID_SEND_RAW_SENSOR_DATA   0xFF02  // direction: input; arguments(uint16): 1, flag (!= 0)
#define SETTING_ID_ENABLE_EHC             0xFF04  // direction: input; arguments(uint16): 1, flag (!= 0)
#define SETTING_ID_AUDIO_ENGINE_CMD       0xFF05  // direction: input; arguments(uint16): 1, command (1:testtone OFF; 2:testtone ON; 3:default sound)

//----- EHC command Ids:
#define EHC_COMMAND_SET_CONTROL_REGISTER 0x0100  // configure a controller
#define EHC_COMMAND_SET_RANGE_MIN        0x0200  // set lower end of ranging
#define EHC_COMMAND_SET_RANGE_MAX        0x0300  // set upper end of ranging
#define EHC_COMMAND_RESET_DELETE         0x0400  // reset or delete a controller

//----- Request Ids:

#define REQUEST_ID_SW_VERSION     0x0000
#define REQUEST_ID_UNMUTE_STATUS  0x0001
#define REQUEST_ID_EHC_DATA       0x0002
#define REQUEST_ID_CLEAR_EEPROM   0x0003
#define REQUEST_ID_COOS_DATA      0x0004
#define REQUEST_ID_EHC_EEPROMSAVE 0x0005

//----- Notification Ids:

#define NOTIFICATION_ID_SW_VERSION     0x0000
#define NOTIFICATION_ID_UNMUTE_STATUS  0x0001
#define NOTIFICATION_ID_EHC_DATA       0x0002
#define NOTIFICATION_ID_CLEAR_EEPROM   0x0003
#define NOTIFICATION_ID_COOS_DATA      0x0004
#define NOTIFICATION_ID_EHC_EEPROMSAVE 0x0005

//----- Mute Status
#define SUP_UNMUTE_STATUS_IS_VALID           (0b1000000000000000)  // status has actually been set
#define SUP_UNMUTE_STATUS_JUMPER_OVERRIDE    (0b0000000010000000)  // hardware jumper overriding everything ...
#define SUP_UNMUTE_STATUS_JUMPER_VALUE       (0b0000000001000000)  // ... with this value (1:unmuted)
#define SUP_UNMUTE_STATUS_SOFTWARE_OVERRIDE  (0b0000000000100000)  // software command overriding midi-derived status ...
#define SUP_UNMUTE_STATUS_SOFTWARE_VALUE     (0b0000000000010000)  // ... with this value (1:unmuted)
#define SUP_UNMUTE_STATUS_MIDI_DERIVED       (0b0000000000001000)  // midi-derived status ...
#define SUP_UNMUTE_STATUS_MIDI_DERIVED_VALUE (0b0000000000000100)  // ... with this value (1:unmuted)
#define SUP_UNMUTE_STATUS_HARDWARE_IS_VALID  (0b0000000000000010)  // hardware status is valid (signal from SUP uC was detected) ...
#define SUP_UNMUTE_STATUS_HARDWARE_VALUE     (0b0000000000000001)  // ... with this value (1:unmuted)

// ===================
#define REQUEST         "req"
#define SW_VERSION      "sw-version"
#define MUTE_STATUS     "mute-status"
#define CLEAR_EEPROM    "clear-eeprom"
#define COOS_DATA       "coos-data"
#define EHC_SAVE_EEPROM "save-ehc"

#define SETTING           "set"
#define MUTE_CTRL         "mute-ctrl"
#define MUTE_CTRL_DISABLE "disable"
#define MUTE_CTRL_MUTE    "mute"
#define MUTE_CTRL_UNMUTE  "unmute"
#define RAW_SENSORS       "sensors"
#define RAW_SENSORS_ON    "on"
#define RAW_SENSORS_OFF   "off"
#define AE_CMD            "ae-cmd"
#define AE_CMD_TTON       "tton"
#define AE_CMD_TTOFF      "ttoff"
#define AE_CMD_DEFSND     "def-snd"

#define KEY_EMUL "key"

uint16_t REQ_DATA[] = { 0x0A00, 0x0001, 0x0000 };
uint16_t SET_DATA[] = { 0x0700, 0x0002, 0x0000, 0x0000 };
uint16_t KEY_DATA[] = { 0x1100, 0x0003, 0x0000, 0x0000, 0x0000 };

// ===================
void Usage(void)
{
  puts("Usage:");
  puts(" lpc  <command>");
  puts("  <commands> : req|set|key");
  puts("  req[uest] : sw-version|mute-status|clear-eeprom|coos-data|save-ehc");
  puts("     sw-version   : get LPC firware version");
  puts("     mute-status  : get software&hardware muting status");
  puts("     clear-eeprom : erase EEPROM");
  puts("     coos-data    : get task scheduler profiling data");
  puts("     save-ehc     : save current EHC config data to EEPROM");
  puts("  set[ting] : mute-ctrl|sensors|ae-cmd");
  puts("     mute-ctrl: disable|mute|unmute : disable mute override or set/clear muting");
  puts("     sensors: on|off                : turn raw sensor messages on/off");
  puts("     ae-cmd: tton|ttoff|def-snd     : Audio Engine Special, test-tone on/off, load default sound");
  puts("  key: <note-nr> <time>         : send emulated key");
  puts("      <note-nr>                     : MIDI key number, 60=\"C3\"");
  puts("      <time>                        : key time (~1/velocity) in ms (2...525), negative means key release");
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
      REQ_DATA[2] = REQUEST_ID_SW_VERSION;
      writeData(driver, sizeof REQ_DATA, &REQ_DATA[0]);
      return 0;
    }
    if (strncmp(argv[2], MUTE_STATUS, sizeof MUTE_STATUS) == 0)
    {
      REQ_DATA[2] = REQUEST_ID_UNMUTE_STATUS;
      writeData(driver, sizeof REQ_DATA, &REQ_DATA[0]);
      return 0;
    }
    if (strncmp(argv[2], CLEAR_EEPROM, sizeof CLEAR_EEPROM) == 0)
    {
      REQ_DATA[2] = REQUEST_ID_CLEAR_EEPROM;
      writeData(driver, sizeof REQ_DATA, &REQ_DATA[0]);
      return 0;
    }
    if (strncmp(argv[2], COOS_DATA, sizeof COOS_DATA) == 0)
    {
      REQ_DATA[2] = REQUEST_ID_COOS_DATA;
      writeData(driver, sizeof REQ_DATA, &REQ_DATA[0]);
      return 0;
    }
    if (strncmp(argv[2], EHC_SAVE_EEPROM, sizeof EHC_SAVE_EEPROM) == 0)
    {
      REQ_DATA[2] = REQUEST_ID_EHC_EEPROMSAVE;
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
      SET_DATA[2] = SETTING_ID_SOFTWARE_MUTE_OVERRIDE;
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
      SET_DATA[2] = SETTING_ID_SEND_RAW_SENSOR_DATA;
      if (strncmp(argv[3], RAW_SENSORS_OFF, sizeof RAW_SENSORS_OFF) == 0)
      {
        SET_DATA[3] = 0;
        writeData(driver, sizeof SET_DATA, &SET_DATA[0]);
        return 0;
      }
      if (strncmp(argv[3], RAW_SENSORS_ON, sizeof RAW_SENSORS_ON) == 0)
      {
        SET_DATA[3] = 1;
        writeData(driver, sizeof SET_DATA, &SET_DATA[0]);
        return 0;
      }
      puts("set sensors : illegal parameter");
      Usage();
    }

    // audio engine command
    if (strncmp(argv[2], AE_CMD, sizeof AE_CMD) == 0)
    {
      SET_DATA[2] = SETTING_ID_AUDIO_ENGINE_CMD;
      if (strncmp(argv[3], AE_CMD_TTOFF, sizeof AE_CMD_TTOFF) == 0)
      {
        SET_DATA[3] = 1;
        writeData(driver, sizeof SET_DATA, &SET_DATA[0]);
        return 0;
      }
      if (strncmp(argv[3], AE_CMD_TTON, sizeof AE_CMD_TTON) == 0)
      {
        SET_DATA[3] = 2;
        writeData(driver, sizeof SET_DATA, &SET_DATA[0]);
        return 0;
      }
      if (strncmp(argv[3], AE_CMD_DEFSND, sizeof AE_CMD_DEFSND) == 0)
      {
        SET_DATA[3] = 3;
        writeData(driver, sizeof SET_DATA, &SET_DATA[0]);
        return 0;
      }
      puts("set ae-cmd : illegal parameter");
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
