#pragma once

enum LPC_BB_MESSAGE_TYPES
{
  LPC_BB_MSG_TYPE_PRESET_DIRECT = 0x0100,  // not used, direction: input; arguments(uint16): N, Nx data
  LPC_BB_MSG_TYPE_MORPH_SET_A   = 0x0200,  // not used
  LPC_BB_MSG_TYPE_MORPH_SET_B   = 0x0300,  // not used
  LPC_BB_MSG_TYPE_PARAMETER     = 0x0400,  // direction: output; arguments(uint16): 2, 1x parameter ID , 1x data
  LPC_BB_MSG_TYPE_EDIT_CONTROL  = 0x0500,  //
  LPC_BB_MSG_TYPE_MORPH_POS     = 0x0600,  // not used
  LPC_BB_MSG_TYPE_SETTING       = 0x0700,  // direction: input;  arguments (uint16): 2, 1x SETTING_ID_*, 1x data
  LPC_BB_MSG_TYPE_NOTIFICATION  = 0x0800,  // direction: output; arguments (uint16): 2, 1x type, 1x value
  LPC_BB_MSG_TYPE_ASSERTION     = 0x0900,  // direction: output; arguments (uint16): n (string)
  LPC_BB_MSG_TYPE_REQUEST       = 0x0A00,  // direction: input;  argument  (uint16): 1, 1x REQUEST_ID_*
  LPC_BB_MSG_TYPE_HEARTBEAT     = 0x0B00,  // direction: output; arguments (uint16): 4, 4x uint16 (==uint64)
  LPC_BB_MSG_TYPE_MUTESTATUS    = 0x0C00,  // direction: output; argument  (uint16): 1, 1x bit pattern
  LPC_BB_MSG_TYPE_RIBBON_CAL    = 0x0D00,  // direction: input;  arguments (uint16): 134, 134x data [2x (33x 34x)]
  LPC_BB_MSG_TYPE_SENSORS_RAW   = 0x0E00,  // direction: output; arguments (uint16): 13, sensor raw data (see nl_tcd_adc_work.c)
  LPC_BB_MSG_TYPE_EHC_CONFIG    = 0x0F00,  // direction: input;  arguments (uint16): 2, 1x command, 1x data
  LPC_BB_MSG_TYPE_EHC_DATA      = 0x1000,  // direction: output; arguments (uint16): n, (see nl_ehc_ctrl.c)
  LPC_BB_MSG_TYPE_KEY_EMUL      = 0x1100,  // direction: input/output;  arguments (uint16): 3, midi key , time(lo), time(high)
  LPC_BB_MSG_TYPE_STAT_DATA     = 0x1200,  // direction: output; arguments (uint16): 4
  LPC_BB_MSG_TYPE_KEYMAP_DATA   = 0x1300,  // direction: input; arguments  (uint16): 32 (for 61 keys)
};

enum LPC_SETTING_IDS
{
  LPC_SETTING_ID_PLAY_MODE_UPPER_RIBBON_BEHAVIOUR = 0,       // ==> BIT 0 set if (returnMode == RETURN), ...
  LPC_SETTING_ID_PLAY_MODE_LOWER_RIBBON_BEHAVIOUR = 1,       // ... BIT 1 set if (touchBehaviour == RELATIVE)
  LPC_SETTING_ID_NOTE_SHIFT                       = 2,       // not used, ==> tTcdRange (-48, 48)
  LPC_SETTING_ID_BASE_UNIT_UI_MODE                = 3,       // ==> PLAY = 0, PARAMETER_EDIT = 1
  LPC_SETTING_ID_EDIT_MODE_RIBBON_BEHAVIOUR       = 4,       // ==> RELATIVE = 0, ABSOLUTE = 1
  LPC_SETTING_ID_PEDAL_1_MODE                     = 5,       // not used, ==> STAY = 0
  LPC_SETTING_ID_PEDAL_2_MODE                     = 6,       // not used, ... RETURN_TO_ZERO = 1
  LPC_SETTING_ID_PEDAL_3_MODE                     = 7,       // not used, ... RETURN_TO_CENTER = 2,
  LPC_SETTING_ID_PEDAL_4_MODE                     = 8,       // not used,
  LPC_SETTING_ID_UPPER_RIBBON_REL_FACTOR          = 9,       // ==> tTcdRange(256, 2560)
  LPC_SETTING_ID_LOWER_RIBBON_REL_FACTOR          = 10,      // ==> tTcdRange(256, 2560)
  LPC_SETTING_ID_VELOCITY_CURVE                   = 11,      // ==> VERY_SOFT = 0, SOFT = 1, NORMAL = 2, HARD = 3, VERY_HARD = 4
  LPC_SETTING_ID_TRANSITION_TIME                  = 12,      // not used, ==> tTcdRange(0, 16000)
  LPC_SETTING_ID_PEDAL_1_TYPE                     = 26,      // ==> PotTipActive  = 0
  LPC_SETTING_ID_PEDAL_2_TYPE                     = 27,      // ... PotRingActive = 1
  LPC_SETTING_ID_PEDAL_3_TYPE                     = 28,      // ... SwitchClosing = 2 // aka momentary switch, normally open
  LPC_SETTING_ID_PEDAL_4_TYPE                     = 29,      // ... SwitchOpening = 3 // aka momentary switch, normally closed
  LPC_SETTING_ID_AFTERTOUCH_CURVE                 = 30,      // SOFT = 0, NORMAL = 1, HARD = 2
  LPC_SETTING_ID_BENDER_CURVE                     = 31,      // SOFT = 0, NORMAL = 1, HARD = 2
  LPC_SETTING_ID_PITCHBEND_ON_PRESSED_KEYS        = 32,      // not used, OFF = 0, ON = 1
  LPC_SETTING_ID_EDIT_SMOOTHING_TIME              = 33,      // not used, ==> tTcdRange(0, 16000)
  LPC_SETTING_ID_PRESET_GLITCH_SUPPRESSION        = 34,      // not used, OFF = 0, ON = 1
  LPC_SETTING_ID_BENDER_RAMP_BYPASS               = 35,      // not used, OFF = 0, ON = 1
  LPC_SETTING_ID_UPPER_RIBBON_VALUE               = 36,      // set initial output value (for relative mode)
  LPC_SETTING_ID_LOWER_RIBBON_VALUE               = 37,      // set initial output value (for relative mode)
  LPC_SETTING_ID_SOFTWARE_MUTE_OVERRIDE           = 0xFF01,  // direction: input; arguments(uint16): 1, mode bit pattern
  LPC_SETTING_ID_SEND_RAW_SENSOR_DATA             = 0xFF02,  // direction: input; arguments(uint16): 1, flag (!= 0)
  LPC_SETTING_ID_ENABLE_KEY_LOGGING               = 0xFF03,  // direction: input; arguments(uint16): 1, flag (!= 0)
  LPC_SETTING_ID_ENABLE_EHC                       = 0xFF04,  // direction: input; arguments(uint16): 1, flag (!= 0)
  LPC_SETTING_ID_AUDIO_ENGINE_CMD                 = 0xFF05,  // direction: input; arguments(uint16): 1, command (1:testtone OFF; 2:testtone ON; 3:default sound)
  LPC_SETTING_ID_SYSTEM_SPECIAL                   = 0xFF06,  // direction: input; arguments(uint16): 1, command (1:reset heartbeat: 2: system reset: 3:Enable MIDI)
  LPC_SETTING_ID_ENABLE_KEY_MAPPING               = 0xFF07,  // direction: input; arguments(uint16): 1, flag (!= 0)
};

enum LPC_REQUEST_IDS
{

  LPC_REQUEST_ID_SW_VERSION     = 0x0000,
  LPC_REQUEST_ID_UNMUTE_STATUS  = 0x0001,
  LPC_REQUEST_ID_EHC_DATA       = 0x0002,
  LPC_REQUEST_ID_CLEAR_EEPROM   = 0x0003,
  LPC_REQUEST_ID_STAT_DATA      = 0x0004,
  LPC_REQUEST_ID_EHC_EEPROMSAVE = 0x0005,
};

enum LPC_NOTIFICATION_IDS
{
  LPC_NOTIFICATION_ID_SW_VERSION     = 0x0000,
  LPC_NOTIFICATION_ID_UNMUTE_STATUS  = 0x0001,
  LPC_NOTIFICATION_ID_EHC_DATA       = 0x0002,
  LPC_NOTIFICATION_ID_CLEAR_EEPROM   = 0x0003,
  LPC_NOTIFICATION_ID_STAT_DATA      = 0x0004,
  LPC_NOTIFICATION_ID_EHC_EEPROMSAVE = 0x0005,
};

enum LPC_EHC_COMMAND_IDS
{
  LPC_EHC_COMMAND_SET_CONTROL_REGISTER = 0x0100,  // configure a controller
  LPC_EHC_COMMAND_SET_RANGE_MIN        = 0x0200,  // set lower end of ranging
  LPC_EHC_COMMAND_SET_RANGE_MAX        = 0x0300,  // set upper end of ranging
  LPC_EHC_COMMAND_RESET_DELETE         = 0x0400,  // reset or delete a controller
  LPC_EHC_COMMAND_FORCE_OUTPUT         = 0x0500,  // force a controller to send current value (if valid)
  LPC_EHC_COMMAND_SET_DEAD_ZONES       = 0x0600,  // set dead zones for auto-ranging
};

enum HW_SOURCE_IDS
{
  HW_SOURCE_ID_PEDAL_1    = 0,
  HW_SOURCE_ID_PEDAL_2    = 1,
  HW_SOURCE_ID_PEDAL_3    = 2,
  HW_SOURCE_ID_PEDAL_4    = 3,
  HW_SOURCE_ID_PITCHBEND  = 4,
  HW_SOURCE_ID_AFTERTOUCH = 5,
  HW_SOURCE_ID_RIBBON_1   = 6,
  HW_SOURCE_ID_RIBBON_2   = 7,
  HW_SOURCE_ID_PEDAL_5    = 8,
  HW_SOURCE_ID_PEDAL_6    = 9,
  HW_SOURCE_ID_PEDAL_7    = 10,
  HW_SOURCE_ID_PEDAL_8    = 11,
  // HW_SOURCE_ID_LAST_KEY is only used to signal last pressed key to BBB (not a real HW_SOURCE for AE)
  // high byte is the physical key number 0..60, low byte is logical note number (36..96, unless hardware-remapped)
  HW_SOURCE_ID_LAST_KEY   = 12,
};
#define NUM_HW_REAL_SOURCES (HW_SOURCE_ID_PEDAL_8 + 1)  // all but LAST_KEY
#define NUM_HW_SOURCES      (HW_SOURCE_ID_LAST_KEY + 1)

enum AE_TCD_OVER_MIDI_IDS
{
  AE_TCD_WRAPPER        = 0x0E,       // USB MIDI packet header "Cable #0, packet type:PitchBend"
  AE_TCD_HW_POS         = 0xE0,       // MIDI command "Pitch Bender" + MIDI channel=HW_SOURCE_ID
  AE_TCD_DEVELOPPER_CMD = 0xE0 + 12,  // MIDI command "Pitch Bender", MIDI channel 12
  AE_TCD_KEY_POS        = 0xE0 + 13,  // MIDI command "Pitch Bender", MIDI channel 13
  AE_TCD_KEY_DOWN       = 0xE0 + 14,  // MIDI command "Pitch Bender", MIDI channel 14
  AE_TCD_KEY_UP         = 0xE0 + 15,  // MIDI command "Pitch Bender", MIDI channel 15
};

enum AE_DEVELOPPER_CMDS
{
  AE_CMD_TONE_OFF      = 1,  // turn off the test tone
  AE_CMD_TONE_ON       = 2,  // turn on the test tone
  AE_CMD_DEFAULT_SOUND = 3,  // set up a simple default sound that is guaranteed to give output
};

enum LPC_SYSTEM_SPECIAL_COMMANDS
{
  SYS_SPECIAL_RESET_HEARTBEAT = 1,
  SYS_SPECIAL_RESET_SYSTEM    = 2,
  SYS_SPECIAL_ENABLE_MIDI     = 3,
};

// void     SUP_SetMuteOverride(uint32_t mode);
// uint16_t SUP_GetUnmuteStatusBits(void);
// bit masks for these functions :
// (for SUP_SetMuteOverride() only *_SOFTWARE_* masks are used/allowed)
enum SUP_MUTING_BITS
{
  SUP_UNMUTE_STATUS_IS_VALID           = 0b1000000000000000,  // status has actually been set
  SUP_UNMUTE_STATUS_JUMPER_OVERRIDE    = 0b0000000010000000,  // hardware jumper overriding everything ...
  SUP_UNMUTE_STATUS_JUMPER_VALUE       = 0b0000000001000000,  // ... with this value (1:unmuted)
  SUP_UNMUTE_STATUS_SOFTWARE_OVERRIDE  = 0b0000000000100000,  // software command overriding midi-derived status ...
  SUP_UNMUTE_STATUS_SOFTWARE_VALUE     = 0b0000000000010000,  // ... with this value (1:unmuted)
  SUP_UNMUTE_STATUS_MIDI_DERIVED       = 0b0000000000001000,  // midi-derived status ...
  SUP_UNMUTE_STATUS_MIDI_DERIVED_VALUE = 0b0000000000000100,  // ... with this value (1:unmuted)
  SUP_UNMUTE_STATUS_HARDWARE_IS_VALID  = 0b0000000000000010,  // hardware status is valid (signal from SUP uC was detected) ...
  SUP_UNMUTE_STATUS_HARDWARE_VALUE     = 0b0000000000000001,  // ... with this value (1:unmuted)
};

typedef struct
{
  unsigned ctrlId : 3;            // controller number 0...7, aka input (main) ADC channel 0...7, 0/1=J1T/R, 2/3=J2T/R, etc,
  unsigned hwId : 4;              // hardware ID used for messages to AE and PG
  unsigned silent : 1;            // disable messaging to AudioEngine
  unsigned is3wire : 1;           // controller connection type, 0=2wire(rheo/sw/cv), 1=3wire(pot)
  unsigned pullup : 1;            // controller input sensing, 0=unloaded(pot/CV), 1=with pullup(rheo/sw)
  unsigned continuous : 1;        // controller output type, 0=continuous(all), 1=bi-stable(all)
  unsigned polarityInvert : 1;    // invert, or don't, the final output(all)
  unsigned autoHoldStrength : 3;  // controller auto-hold 0..7, 0(off)...4=autoHold-Strength for pot/rheo
  unsigned doAutoRanging : 1;     // enable auto-ranging, or assume static (but adjustable) thresholds/levels
} EHC_ControllerConfig_T;

typedef struct
{
  unsigned initialized : 1;    // controller is initialized (has valid setup)
  unsigned pluggedIn : 1;      // controller is plugged in
  unsigned isReset : 1;        // controller is freshly reset
  unsigned outputIsValid : 1;  // controller final output value has been set
  unsigned isAutoRanged : 1;   // controller has finished auto-ranging (always=1 when disabled)
  unsigned isSettled : 1;      // controller output is within 'stable' bounds and step-freezing (not valid for bi-stable)
  unsigned isRamping : 1;      // controller currently does a ramp to the actual value (pot/rheo) (not valid for bi-stable)
  unsigned isSaved : 1;        // controller state has been saved to EEPROM
  unsigned isRestored : 1;     // controller state has been restored from EEPROM

} EHC_ControllerStatus_T;
