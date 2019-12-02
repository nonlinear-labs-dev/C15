/*
 * nl_param.h
 *
 *  Created on: 30.01.2015
 *      Author: ssc
 */

#ifndef NL_TCD_PARAM_WORK_H_
#define NL_TCD_PARAM_WORK_H_

#include "stdint.h"

//======== defines

#define NUM_PCS        8    // number of play controls
#define NUM_MCS        4    // number of macro controls
#define NUM_UI_PARAMS  324  // parameters in a preset
#define NUM_MC_TARGETS 87   // number of parameters that can be assigned to an MC

//====== UI parameters

//------ Basic Parameters

#define PARAM_ID_ENV_A_GAIN 12  // bipolar
#define PARAM_ID_ENV_B_GAIN 31  // bipolar

#define PARAM_ID_OSC_A_PITCH          53
#define PARAM_ID_OSC_A_KEY_TRK        55
#define PARAM_ID_OSC_A_PITCH_ENV      56  // bipolar
#define PARAM_ID_OSC_A_FLUCT          57
#define PARAM_ID_OSC_A_FLUCT_ENV      59  // bipolar
#define PARAM_ID_OSC_A_PM_SELF        60  // bipolar
#define PARAM_ID_OSC_A_PM_SELF_ENV    62
#define PARAM_ID_OSC_A_PM_SELF_SHAPER 63  // bipolar
#define PARAM_ID_OSC_A_PM_B           64  // bipolar
#define PARAM_ID_OSC_A_PM_B_ENV       66
#define PARAM_ID_OSC_A_PM_B_SHAPER    67  // bipolar
#define PARAM_ID_OSC_A_PM_FB          68  // bipolar
#define PARAM_ID_OSC_A_PM_FB_ENV      70

#define PARAM_ID_SHAPER_A_DRIVE     71
#define PARAM_ID_SHAPER_A_DRIVE_ENV 73
#define PARAM_ID_SHAPER_A_FOLD      74
#define PARAM_ID_SHAPER_A_ASYM      75
#define PARAM_ID_SHAPER_A_MIX       76  // bipolar
#define PARAM_ID_SHAPER_A_FB_MIX    78
#define PARAM_ID_SHAPER_A_FB_ENV    80
#define PARAM_ID_SHAPER_A_RING_MOD  81

#define PARAM_ID_OSC_B_PITCH          83
#define PARAM_ID_OSC_B_KEY_TRK        85
#define PARAM_ID_OSC_B_PITCH_ENV      86  // bipolar
#define PARAM_ID_OSC_B_FLUCT          87
#define PARAM_ID_OSC_B_FLUCT_ENV      89  // bipolar
#define PARAM_ID_OSC_B_PM_SELF        90  // bipolar
#define PARAM_ID_OSC_B_PM_SELF_ENV    92
#define PARAM_ID_OSC_B_PM_SELF_SHAPER 93  // bipolar
#define PARAM_ID_OSC_B_PM_A           94  // bipolar
#define PARAM_ID_OSC_B_PM_A_ENV       96
#define PARAM_ID_OSC_B_PM_A_SHAPER    97  // bipolar
#define PARAM_ID_OSC_B_PM_FB          98  // bipolar
#define PARAM_ID_OSC_B_PM_FB_ENV      100

#define PARAM_ID_SHAPER_B_DRIVE     101
#define PARAM_ID_SHAPER_B_MIX       106
#define PARAM_ID_SHAPER_B_DRIVE     101
#define PARAM_ID_SHAPER_B_DRIVE_ENV 103
#define PARAM_ID_SHAPER_B_FOLD      104
#define PARAM_ID_SHAPER_B_ASYM      105
#define PARAM_ID_SHAPER_B_MIX       106  // bipolar
#define PARAM_ID_SHAPER_B_FB_MIX    108
#define PARAM_ID_SHAPER_B_FB_ENV    100
#define PARAM_ID_SHAPER_B_RING_MOD  111

#define PARAM_ID_COMB_A_B            113
#define PARAM_ID_COMB_PITCH          115
#define PARAM_ID_COMB_PITCH_KEY_TRK  117
#define PARAM_ID_COMB_PITCH_ENV      118  // bipolar
#define PARAM_ID_COMB_DECAY          119  // bipolar
#define PARAM_ID_COMB_DECAY_KEY_TRK  121
#define PARAM_ID_COMB_DECAY_GATE     305
#define PARAM_ID_COMB_AP_TUNE        123
#define PARAM_ID_COMB_AP_KEY_TRK     125
#define PARAM_ID_COMB_AP_ENV         126  // bipolar
#define PARAM_ID_COMB_AP_RESON       127
#define PARAM_ID_COMB_HI_CUT         129
#define PARAM_ID_COMB_HI_CUT_KEY_TRK 131
#define PARAM_ID_COMB_HI_CUT_ENV     132
#define PARAM_ID_COMB_PM             133  // bipolar
#define PARAM_ID_COMB_PM_A_B         135

#define PARAM_ID_SVF_A_B            136
#define PARAM_ID_SVF_COMB_MIX       138  // bipolar
#define PARAM_ID_SVF_CUTOFF         140
#define PARAM_ID_SVF_CUTOFF_KEY_TRK 142
#define PARAM_ID_SVF_CUTOFF_ENV     143  // bipolar
#define PARAM_ID_SVF_RESON          144
#define PARAM_ID_SVF_RESON_KEY_TRK  146  // bipolar
#define PARAM_ID_SVF_RESON_ENV      147  // bipolar
#define PARAM_ID_SVF_SPREAD         148  // bipolar
#define PARAM_ID_SVF_L_B_H          150
#define PARAM_ID_SVF_PARALLEL       152  // bipolar
#define PARAM_ID_SVF_FM             153  // bipolar
#define PARAM_ID_SVF_FM_A_B         155

#define PARAM_ID_FB_MIXER_COMB    156  // bipolar
#define PARAM_ID_FB_MIXER_SVF     158  // bipolar
#define PARAM_ID_FB_MIXER_EFFECTS 160  // bipolar
#define PARAM_ID_FB_MIXER_REVERB  162
#define PARAM_ID_FB_MIXER_DRIVE   164
#define PARAM_ID_FB_MIXER_FOLD    166
#define PARAM_ID_FB_MIXER_ASYM    167
#define PARAM_ID_FB_MIXER_LEVEL   299
#define PARAM_ID_FB_MIXER_LVL_KT  168  // bipolar

#define PARAM_ID_OUT_MIXER_A        169  // bipolar
#define PARAM_ID_OUT_MIXER_A_PAN    171  // bipolar
#define PARAM_ID_OUT_MIXER_B        172  // bipolar
#define PARAM_ID_OUT_MIXER_B_PAN    174  // bipolar
#define PARAM_ID_OUT_MIXER_COMB     175  // bipolar
#define PARAM_ID_OUT_MIXER_COMB_PAN 177  // bipolar
#define PARAM_ID_OUT_MIXER_SVF      178  // bipolar
#define PARAM_ID_OUT_MIXER_SVF_PAN  180  // bipolar
#define PARAM_ID_OUT_MIXER_DRIVE    181
#define PARAM_ID_OUT_MIXER_FOLD     183
#define PARAM_ID_OUT_MIXER_ASYM     184
#define PARAM_ID_OUT_MIXER_LEVEL    185
#define PARAM_ID_OUT_MIXER_KEY_PAN  187

#define PARAM_ID_CABINET_DRIVE  188
#define PARAM_ID_CABINET_FOLD   190
#define PARAM_ID_CABINET_ASYM   191
#define PARAM_ID_CABINET_TILT   192  // bipolar
#define PARAM_ID_CABINET_HI_CUT 194
#define PARAM_ID_CABINET_LO_CUT 196
#define PARAM_ID_CABINET_LEVEL  197
#define PARAM_ID_CABINET_MIX    199

#define PARAM_ID_GAP_FILT_CENTER  201
#define PARAM_ID_GAP_FILT_STEREO  203  // bipolar
#define PARAM_ID_GAP_FILT_GAP     204
#define PARAM_ID_GAP_FILT_RESON   206
#define PARAM_ID_GAP_FILT_BALANCE 207  // bipolar
#define PARAM_ID_GAP_FILT_MIX     209  // bipolar

#define PARAM_ID_FLANGER_T_MOD    211  /// vorher "_DEPTH" und unipolar
#define PARAM_ID_FLANGER_PHASE    213
#define PARAM_ID_FLANGER_RATE     214
#define PARAM_ID_FLANGER_TIME     216
#define PARAM_ID_FLANGER_STEREO   218  // bipolar
#define PARAM_ID_FLANGER_FB       219  // bipolar
#define PARAM_ID_FLANGER_CROSS_FB 221  // bipolar - 07.06.2017
#define PARAM_ID_FLANGER_HI_CUT   222
#define PARAM_ID_FLANGER_MIX      223  // bipolar
#define PARAM_ID_FLANGER_ENV_F    307  /// neu
#define PARAM_ID_FLANGER_AP_MOD   308  // bipolar	/// neu
#define PARAM_ID_FLANGER_AP_TUNE  310  /// neu

#define PARAM_ID_ECHO_TIME     225
#define PARAM_ID_ECHO_STEREO   227  // bipolar
#define PARAM_ID_ECHO_FB       229
#define PARAM_ID_ECHO_CROSS_FB 231
#define PARAM_ID_ECHO_HI_CUT   232
#define PARAM_ID_ECHO_MIX      233

#define PARAM_ID_REVERB_SIZE   235
#define PARAM_ID_REVERB_CHORUS 237
#define PARAM_ID_REVERB_HI_CUT 238
#define PARAM_ID_REVERB_LO_CUT 240
#define PARAM_ID_REVERB_MIX    241

#define PARAM_ID_UNISON_DETUNE 250

//------ Play Controls

#define PARAM_ID_PEDAL_1    254
#define PARAM_ID_PEDAL_2    259
#define PARAM_ID_PEDAL_3    264
#define PARAM_ID_PEDAL_4    269
#define PARAM_ID_PITCHBEND  274
#define PARAM_ID_AFTERTOUCH 279
#define PARAM_ID_RIBBON_1   284
#define PARAM_ID_RIBBON_2   289

//------ Play Control Amounts

#define PARAM_ID_PEDAL_1_TO_MC_A    255
#define PARAM_ID_PEDAL_1_TO_MC_B    256
#define PARAM_ID_PEDAL_1_TO_MC_C    257
#define PARAM_ID_PEDAL_1_TO_MC_D    258
#define PARAM_ID_PEDAL_2_TO_MC_A    260
#define PARAM_ID_PEDAL_2_TO_MC_B    261
#define PARAM_ID_PEDAL_2_TO_MC_C    262
#define PARAM_ID_PEDAL_2_TO_MC_D    263
#define PARAM_ID_PEDAL_3_TO_MC_A    265
#define PARAM_ID_PEDAL_3_TO_MC_B    266
#define PARAM_ID_PEDAL_3_TO_MC_C    267
#define PARAM_ID_PEDAL_3_TO_MC_D    268
#define PARAM_ID_PEDAL_4_TO_MC_A    270
#define PARAM_ID_PEDAL_4_TO_MC_B    271
#define PARAM_ID_PEDAL_4_TO_MC_C    272
#define PARAM_ID_PEDAL_4_TO_MC_D    273
#define PARAM_ID_PITCHBEND_TO_MC_A  275
#define PARAM_ID_PITCHBEND_TO_MC_B  276
#define PARAM_ID_PITCHBEND_TO_MC_C  277
#define PARAM_ID_PITCHBEND_TO_MC_D  278
#define PARAM_ID_AFTERTOUCH_TO_MC_A 280
#define PARAM_ID_AFTERTOUCH_TO_MC_B 281
#define PARAM_ID_AFTERTOUCH_TO_MC_C 282
#define PARAM_ID_AFTERTOUCH_TO_MC_D 283
#define PARAM_ID_RIBBON_1_TO_MC_A   285
#define PARAM_ID_RIBBON_1_TO_MC_B   286
#define PARAM_ID_RIBBON_1_TO_MC_C   287
#define PARAM_ID_RIBBON_1_TO_MC_D   288
#define PARAM_ID_RIBBON_2_TO_MC_A   290
#define PARAM_ID_RIBBON_2_TO_MC_B   291
#define PARAM_ID_RIBBON_2_TO_MC_C   292
#define PARAM_ID_RIBBON_2_TO_MC_D   293

//------- Macro Controls

#define PARAM_ID_MACRO_CONTROL_A 243
#define PARAM_ID_MACRO_CONTROL_B 244
#define PARAM_ID_MACRO_CONTROL_C 245
#define PARAM_ID_MACRO_CONTROL_D 246

//------- Macro Control Amounts

#define PARAM_ID_ENV_A_ATTACK_MC     1
#define PARAM_ID_ENV_A_DECAY_1_MC    3
#define PARAM_ID_ENV_A_BREAKPOINT_MC 5
#define PARAM_ID_ENV_A_DECAY_2_MC    7
#define PARAM_ID_ENV_A_SUSTAIN_MC    9
#define PARAM_ID_ENV_A_RELEASE_MC    11
#define PARAM_ID_ENV_A_GAIN_MC       13

#define PARAM_ID_ENV_B_ATTACK_MC     20
#define PARAM_ID_ENV_B_DECAY_1_MC    22
#define PARAM_ID_ENV_B_BREAKPOINT_MC 24
#define PARAM_ID_ENV_B_DECAY_2_MC    26
#define PARAM_ID_ENV_B_SUSTAIN_MC    28
#define PARAM_ID_ENV_B_RELEASE_MC    30
#define PARAM_ID_ENV_B_GAIN_MC       32

#define PARAM_ID_ENV_C_ATTACK_MC     39
#define PARAM_ID_ENV_C_DECAY_1_MC    41
#define PARAM_ID_ENV_C_BREAKPOINT_MC 43
#define PARAM_ID_ENV_C_DECAY_2_MC    45
#define PARAM_ID_ENV_C_SUSTAIN_MC    298
#define PARAM_ID_ENV_C_RELEASE_MC    47

#define PARAM_ID_OSC_A_PITCH_MC   54
#define PARAM_ID_OSC_A_FLUCT_MC   58
#define PARAM_ID_OSC_A_PM_SELF_MC 61
#define PARAM_ID_OSC_A_PM_B_MC    65
#define PARAM_ID_OSC_A_PM_FB_MC   69

#define PARAM_ID_SHAPER_A_DRIVE_MC    72
#define PARAM_ID_SHAPER_A_MIX_MC      77
#define PARAM_ID_SHAPER_A_FB_MIX_MC   79
#define PARAM_ID_SHAPER_A_RING_MOD_MC 82

#define PARAM_ID_OSC_B_PITCH_MC   84
#define PARAM_ID_OSC_B_FLUCT_MC   88
#define PARAM_ID_OSC_B_PM_SELF_MC 91
#define PARAM_ID_OSC_B_PM_A_MC    95
#define PARAM_ID_OSC_B_PM_FB_MC   99

#define PARAM_ID_SHAPER_B_DRIVE_MC    102
#define PARAM_ID_SHAPER_B_MIX_MC      107
#define PARAM_ID_SHAPER_B_FB_MIX_MC   109
#define PARAM_ID_SHAPER_B_RING_MOD_MC 112

#define PARAM_ID_COMB_A_B_MC        114
#define PARAM_ID_COMB_PITCH_MC      116
#define PARAM_ID_COMB_DECAY_MC      120
#define PARAM_ID_COMB_DECAY_GATE_MC 306
#define PARAM_ID_COMB_AP_TUNE_MC    124
#define PARAM_ID_COMB_AP_RESON_MC   128
#define PARAM_ID_COMB_HI_CUT_MC     130
#define PARAM_ID_COMB_PM_MC         134

#define PARAM_ID_SVF_A_B_MC      137
#define PARAM_ID_SVF_COMB_MIX_MC 139
#define PARAM_ID_SVF_CUTOFF_MC   141
#define PARAM_ID_SVF_RESON_MC    145
#define PARAM_ID_SVF_SPREAD_MC   149
#define PARAM_ID_SVF_L_B_H_MC    151
#define PARAM_ID_SVF_FM_MC       154

#define PARAM_ID_FB_MIXER_COMB_MC    157
#define PARAM_ID_FB_MIXER_SVF_MC     159
#define PARAM_ID_FB_MIXER_EFFECTS_MC 161
#define PARAM_ID_FB_MIXER_REVERB_MC  163
#define PARAM_ID_FB_MIXER_DRIVE_MC   165
#define PARAM_ID_FB_MIXER_LEVEL_MC   300

#define PARAM_ID_OUT_MIXER_A_MC     170
#define PARAM_ID_OUT_MIXER_B_MC     173
#define PARAM_ID_OUT_MIXER_COMB_MC  176
#define PARAM_ID_OUT_MIXER_SVF_MC   179
#define PARAM_ID_OUT_MIXER_DRIVE_MC 182
#define PARAM_ID_OUT_MIXER_LEVEL_MC 186

#define PARAM_ID_CABINET_DRIVE_MC  189
#define PARAM_ID_CABINET_TILT_MC   193
#define PARAM_ID_CABINET_HI_CUT_MC 195
#define PARAM_ID_CABINET_LEVEL_MC  198
#define PARAM_ID_CABINET_MIX_MC    200

#define PARAM_ID_GAP_FILT_CENTER_MC  202
#define PARAM_ID_GAP_FILT_GAP_MC     205
#define PARAM_ID_GAP_FILT_BALANCE_MC 208
#define PARAM_ID_GAP_FILT_MIX_MC     210

#define PARAM_ID_FLANGER_T_MOD_MC   212
#define PARAM_ID_FLANGER_RATE_MC    215
#define PARAM_ID_FLANGER_TIME_MC    217
#define PARAM_ID_FLANGER_FB_MC      220
#define PARAM_ID_FLANGER_MIX_MC     224
#define PARAM_ID_FLANGER_AP_MOD_MC  309  /// neu
#define PARAM_ID_FLANGER_AP_TUNE_MC 311  /// neu

#define PARAM_ID_ECHO_TIME_MC   226
#define PARAM_ID_ECHO_STEREO_MC 228
#define PARAM_ID_ECHO_FB_MC     230
#define PARAM_ID_ECHO_MIX_MC    234

#define PARAM_ID_REVERB_SIZE_MC   236
#define PARAM_ID_REVERB_HI_CUT_MC 239
#define PARAM_ID_REVERB_MIX_MC    242

#define PARAM_ID_UNISON_DETUNE_MC 251

//------- Microtuning Parameters

#define PARAM_ID_SCALE_BASE_KEY  312
#define PARAM_ID_SCALE_OFFSET_1  313
#define PARAM_ID_SCALE_OFFSET_2  314
#define PARAM_ID_SCALE_OFFSET_3  315
#define PARAM_ID_SCALE_OFFSET_4  316
#define PARAM_ID_SCALE_OFFSET_5  317
#define PARAM_ID_SCALE_OFFSET_6  318
#define PARAM_ID_SCALE_OFFSET_7  319
#define PARAM_ID_SCALE_OFFSET_8  320
#define PARAM_ID_SCALE_OFFSET_9  321
#define PARAM_ID_SCALE_OFFSET_10 322
#define PARAM_ID_SCALE_OFFSET_11 323

#define PARAM_ID_ENV_A_ATTACK_TIME      0
#define PARAM_ID_ENV_A_ATTACK_CURVE     294
#define PARAM_ID_ENV_A_DECAY_1_TIME     2
#define PARAM_ID_ENV_A_BREAKPOINT_LEVEL 4
#define PARAM_ID_ENV_A_DECAY_2_TIME     6
#define PARAM_ID_ENV_A_SUSTAIN_LEVEL    8
#define PARAM_ID_ENV_A_RELEASE_TIME     10
#define PARAM_ID_ENV_A_LEVELS_VELOCITY  14
#define PARAM_ID_ENV_A_ATTACK_VELOCITY  15
#define PARAM_ID_ENV_A_RELEASE_VELOCITY 16
#define PARAM_ID_ENV_A_LEVELS_KEY_TRK   17
#define PARAM_ID_ENV_A_TIMES_KEY_TRK    18

#define PARAM_ID_ENV_B_ATTACK_TIME      19
#define PARAM_ID_ENV_B_ATTACK_CURVE     295
#define PARAM_ID_ENV_B_DECAY_1_TIME     21
#define PARAM_ID_ENV_B_BREAKPOINT_LEVEL 23
#define PARAM_ID_ENV_B_DECAY_2_TIME     25
#define PARAM_ID_ENV_B_SUSTAIN_LEVEL    27
#define PARAM_ID_ENV_B_RELEASE_TIME     29
#define PARAM_ID_ENV_B_LEVELS_VELOCITY  33
#define PARAM_ID_ENV_B_ATTACK_VELOCITY  34
#define PARAM_ID_ENV_B_RELEASE_VELOCITY 35
#define PARAM_ID_ENV_B_LEVELS_KEY_TRK   36
#define PARAM_ID_ENV_B_TIMES_KEY_TRK    37

#define PARAM_ID_ENV_C_ATTACK_TIME      38
#define PARAM_ID_ENV_C_ATTACK_CURVE     296
#define PARAM_ID_ENV_C_DECAY_1_TIME     40
#define PARAM_ID_ENV_C_BREAKPOINT_LEVEL 42
#define PARAM_ID_ENV_C_DECAY_2_TIME     44
#define PARAM_ID_ENV_C_SUSTAIN_LEVEL    297
#define PARAM_ID_ENV_C_RELEASE_TIME     46
#define PARAM_ID_ENV_C_LEVELS_VELOCITY  48
#define PARAM_ID_ENV_C_ATTACK_VELOCITY  49
#define PARAM_ID_ENV_C_RELEASE_VELOCITY 50
#define PARAM_ID_ENV_C_LEVELS_KEY_TRK   51
#define PARAM_ID_ENV_C_TIMES_KEY_TRK    52

#define PARAM_ID_OSC_A_PHASE 301
#define PARAM_ID_OSC_B_PHASE 302

#define PARAM_ID_KEY_PAN 187

#define PARAM_ID_MASTER_TUNE 248

#define PARAM_ID_UNISON_VOICES 249
#define PARAM_ID_UNISON_DETUNE 250
#define PARAM_ID_UNISON_PHASE  252
#define PARAM_ID_UNISON_PAN    253

#define PARAM_ID_NOTE_SHIFT 338

//----- Non-UI parameters

#define PARAM_ID_ENV_C_RATE 408
#define PARAM_ID_NOTE_ON_VS 409

#define PARAM_ID_FADE_POINT 415

//======== public functions

void PARAM_WORK_Init(void);

void PARAM_Set(uint32_t paramId, uint32_t paramVal);
void PARAM_Set2(uint32_t paramId, uint32_t paramVal1, uint32_t paramVal2);
void PARAM_ApplyPreset(uint16_t length, uint16_t* data);

void PARAM_SetTransitionTime(uint32_t time);
void PARAM_SetEditSmoothingTime(uint32_t time);
void PARAM_SetGlitchSuppression(uint32_t mode);

void PARAM_SetNoteShift(uint32_t shift);

#endif /* NL_TCD_PARAM_WORK_H_ */
