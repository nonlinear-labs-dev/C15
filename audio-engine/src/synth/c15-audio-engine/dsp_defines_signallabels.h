/******************************************************************************/
/** @file       dsp_defines_signallabels.h
    @date       2018-08-21
    @version    1.0
    @author     Matthias Seeber & Anton Schmied
    @brief      Defines for the individual Signal Names (indices) in the great
                shared m_paramsignaldata[][] array
*******************************************************************************/

#pragma once

enum class Signals
{

  /* Envelope Signals                 -- polyphonic */
  Invalid = -1,
  Unused = 16383,

  ENV_A_MAG = 0,   // Envelope A (magnitude component)
  ENV_A_TMB,   // Envelope A (timbre component)
  ENV_B_MAG,   // Envelope B (magnitude component)
  ENV_B_TMB,   // Envelope B (timbre component)
  ENV_C_CLIP,  // Envelope C (clipped)
  ENV_C_UNCL,  // Envelope C (unclipped)
  ENV_G_SIG,   // Gate Envelope

  /* Oscillator A Signals             -- polyphonic */

  OSC_A_FRQ,  // Oscillator A Frequency (Pitch, Pitch KT, Pitch Env C Amount, Env C, Master Tune, Reference, Key Pitch)
  OSC_A_FLUEC,   // Oscillator A Fluctuation (Amount, Env C Amount, Env C)
  OSC_A_PMSEA,   // Oscillator A PM Self (Amount, Env A Amount, Env A)
  OSC_A_PMSSH,  // Oscillator A PM Self Shaper (Amount)
  OSC_A_PMBEB,  // Oscillator A PM B (Amount, Env B Amount, Env B)
  OSC_A_PMBSH,  // Oscillator A PM B Shaper (Amount)
  OSC_A_PMFEC,  // Oscillator A PM FB (Amount, Env C Amount, Env C)
  OSC_A_PHS,    // Oscillator A Phase (Offset)
  OSC_A_CHI,    // Oscillator A Chirp Frequency (Cutoff)

  /* Shaper A Signals                 -- polyphonic */

  SHP_A_DRVEA,  // Shaper A Drive (Drive, Env A Amount, Env A)
  SHP_A_FLD,    // Shaper A Fold (Amount)
  SHP_A_ASM,    // Shaper A Asymetry (Amount)
  SHP_A_MIX,    // Shaper A Mix (Amount)
  SHP_A_FBM,    // Shaper A Feedback Mix (Amount)
  SHP_A_FBEC,   // Shaper A Feedback Env C (Amount, Env C, Gate)
  SHP_A_RM,     // Shaper A Ringmod (Amount)

  /* Oscillator B Signals             -- polyphonic */

  OSC_B_FRQ,  // Oscillator B Frequency (Pitch, Pitch KT, Pitch Env C Amount, Env C, Master Tune, Reference, Key Pitch)
  OSC_B_FLUEC,  // Oscillator B Fluctuation (Amount, Env C Amount, Env C)
  OSC_B_PMSEB,  // Oscillator B PM Self (Amount, Env B Amount, Env B)
  OSC_B_PMSSH,  // Oscillator B PM Self Shaper (Amount)
  OSC_B_PMAEA,  // Oscillator B PM A (Amount, Env A Amount, Env A)
  OSC_B_PMASH,  // Oscillator B PM A Shaper (Amount)
  OSC_B_PMFEC,  // Oscillator B PM FB (Amount, Env C Amount, Env C)
  OSC_B_PHS,    // Oscillator B Phase (Offset)
  OSC_B_CHI,    // Oscillator B Chirp Frequency (Cutoff)

  /* Shaper B Signals                 -- polyphonic */

  SHP_B_DRVEB,  // Shaper B Drive (Drive, Env B Amount, Env B)
  SHP_B_FLD,    // Shaper B Fold (Amount)
  SHP_B_ASM,    // Shaper B Asymetry (Amount)
  SHP_B_MIX,    // Shaper B Mix (Amount)
  SHP_B_FBM,    // Shaper B Feedback Mix (Amount)
  SHP_B_FBEC,   // Shaper B Feedback Env C (Amount, Env C, Gate)
  SHP_B_RM,     // Shaper B Ringmod (Amount)

  /* Comb Filter Signals              -- polyphonic */

  CMB_AB,    // Comb AB Input Mix (Amount)
  CMB_FRQ,   // Comb Frequency (Pitch, Pitch KT, Master Tune, Reference, Key Pitch)
  CMB_BYP,   // Comb Bypass (Pitch)
  CMB_FEC,   // Comb Pitch Env C (Frequency Factor)
  CMB_DEC,   // Comb Decay (Decay, Decay KT, Decay Gate Amount, Gate, Master Tune, Key Pitch)
  CMB_APF,   // Comb Allpass Frequency (Tune, AP KT, AP Env C Amount, Env C, Master Tune, Key Pitch)
  CMB_APR,   // Comb Allpass Resonance (Amount)
  CMB_LPF,   // Comb Hi Cut Frequency (Hi Cut, Hi Cut KT, Hi Cut Env C, Env C, Master Tune, Key Pitch)
  CMB_PM,    // Comb PM (Amount)
  CMB_PMAB,  // Comb PM AB Mix (Amount)

  /* State Variable Filter Signals    -- polyphonic */

  SVF_AB,    // SVF AB Input Mix (Amount)
  SVF_CMIX,  // SVF Comb Input Mix (Amount)
  SVF_F1_CUT,  // SVF Upper Filter Frequency (Cutoff, Cutoff KT, Cutoff Env C Amount, Env C, Spread, Master Tune, Reference, Key Pitch)
  SVF_F2_CUT,  // SVF Lower Filter Frequency (Cutoff, Cutoff KT, Cutoff Env C Amount, Env C, Spread, Master Tune, Reference, Key Pitch)
  SVF_F1_FM,  // SVF Upper Filter FM Amount (Cutoff, Cutoff KT, Cutoff Env C Amount, Env C, Spread, FM, Master Tune, Reference, Key Pitch)
  SVF_F2_FM,  // SVF Lower Filter FM Amount (Cutoff, Cutoff KT, Cutoff Env C Amount, Env C, Spread, FM, Master Tune, Reference, Key Pitch)
  SVF_RES,    // SVF Resonance (old) (Resonance, Res KT, Res Env C Amount, Env C, Master Tune, Key Pitch)
  SVF_RES_DAMP,    // SVF Resonance (new) - damp factor (derived from res in param engine)
  SVF_RES_FMAX,    // SVF Resonance (new) - freq maximum (derived from res in param engine)
  SVF_LBH_1,  // SVF Upper LBH Mix (Amount)
  SVF_LBH_2,  // SVF Lower LBH Mix (Amount)
  SVF_PAR_1,  // SVF Parallel Amount 1 (Amount)
  SVF_PAR_2,  // SVF Parallel Amount 2 (Amount)
  SVF_PAR_3,  // SVF Parallel Amount 3 (Amount)
  SVF_PAR_4,  // SVF Parallel Amount 4 (Amount)
  SVF_FMAB,   // SVF FM AB Mix (Amount)

  /* Feedback Mixer Signals           -- polyphonic */

  FBM_CMB,  // FB Mix Comb Mix (Amount)
  FBM_SVF,  // FB Mix SVF Mix (Amount)
  FBM_FX,   // FB Mix Effects Mix (Amount)
  FBM_REV,  // FB Mix Reverb Mix (Amount)
  FBM_DRV,  // FB Mix Drive (Amount)
  FBM_FLD,  // FB Mix Fold (Amount)
  FBM_ASM,  // FB Mix Asymetry (Amount)
  FBM_LVL,  // FB Mix Level (Level, Level KT, Master Tune, Key Pitch)
  FBM_HPF,  // FB Mix Highpass Frequency (Master Tune, Key Pitch)

  /* Output Mixer Signals             -- polyphonic */

  OUT_A_L,    // Out Mix Branch A Left Level (Level, Pan, Key Pan)
  OUT_A_R,    // Out Mix Branch A Right Level (Level, Pan, Key Pan)
  OUT_B_L,    // Out Mix Branch B Left Level (Level, Pan, Key Pan)
  OUT_B_R,    // Out Mix Branch B Right Level (Level, Pan, Key Pan)
  OUT_CMB_L,  // Out Mix Comb Left Level (Level, Pan, Key Pan)
  OUT_CMB_R,  // Out Mix Comb Right Level (Level, Pan, Key Pan)
  OUT_SVF_L,  // Out Mix SVF Left Level (Level, Pan, Key Pan)
  OUT_SVF_R,  // Out Mix SVF Right Level (Level, Pan, Key Pan)
  OUT_DRV,    // Out Mix Drive (Amount)
  OUT_FLD,    // Out Mix Fold (Amount)
  OUT_ASM,    // Out Mix Asymetry (Amount)
  OUT_LVL,    // Out Mix Level (Level)

  /* Cabinet Signals                  -- monophonic */

  CAB_DRV,     // Cabinet Drive (Amount)
  CAB_FLD,     // Cabinet Fold (Amount)
  CAB_ASM,     // Cabinet Asymetry (Amount)
  CAB_PRESAT,  // Cabinet Saturation Level (Tilt - pre Shaper factor)
  CAB_SAT,     // Cabinet Saturation Level (Tilt - post Shaper factor)
  CAB_TILT,    // Cabinet Tilt (Tilt)
  CAB_LPF,     // Cabinet Hi Cut (Cutoff)
  CAB_HPF,     // Cabinet Lo Cut (Cutoff)
  CAB_DRY,     // Cabinet Dry Amount (Mix)
  CAB_WET,     // Cabinet Wet Amount (Mix)

  /* Gap Filter Signals               -- monophonic */

  GAP_LFL,     // Gap Filter Left Lowpass Frequency (Center, Stereo, Gap)
  GAP_HFL,     // Gap Filter Left Highpass Frequency (Center, Stereo, Gap)
  GAP_LFR,     // Gap Filter Right Lowpass Frequency (Center, Stereo, Gap)
  GAP_HFR,     // Gap Filter Right Highpass Frequency (Center, Stereo, Gap)
  GAP_RES,     // Gap Filter Resonance (Amount)
  GAP_HPLP,    // Gap Filter Highpass to Lowpass Amount (Balance, Mix)
  GAP_INLP,   // Gap Filter Input to Lowpass Amount (Balance, Mix)
  GAP_HPOUT,  // Gap Filter Highpass to Output Amount (Balance, Mix)
  GAP_LPOUT,  // Gap Filter Lowpass to Output Amount (Balance, Mix)
  GAP_INOUT,  // Gap Filter Input to Output Amount (Balance, Mix)

  /* Flanger Signals                  -- monophonic */

  FLA_TMOD,    // Flanger Time Modulation (Amount)
  FLA_LFO_L,   // Flanger Left Modulation Signal (Phase, Rate, Envelope, Flanger LFO/Envelope)
  FLA_LFO_R,   // Flanger Right Modulation Signal (Phase, Rate, Envelope, Flanger LFO/Envelope)
  FLA_TL,      // Flanger Left Time (Time, Stereo)
  FLA_TR,      // Flanger Right Time (Time, Stereo)
  FLA_FB_LOC,  // Flanger Local Feedback Amount (Feedback, Cross FB)
  FLA_FB_CR,   // Flanger Cross Feedback Amount (Feedback, Cross FB)
  FLA_LPF,     // Flanger Lowpass Frequency (Hi Cut)
  FLA_DRY,     // Flanger Dry Amount (Mix)
  FLA_WET,     // Flanger Wet Amount (Mix)
  FLA_APF_L,   // Flanger Left Allpass Frequency (AP Mod, AP Tune, Flanger LFO/Envelope)
  FLA_APF_R,   // Flanger Right Allpass Frequency (AP Mod, AP Tune, Flanger LFO/Envelope)

  /* Echo Signals                     -- monophonic */

  DLY_TL,      // Echo Left Time (Time, Stereo)
  DLY_TR,      // Echo Right Time (Time, Stereo)
  DLY_FB_LOC,  // Echo Local Feedback (Feedback, Cross FB)
  DLY_FB_CR,   // Echo Cross Feedback (Feedback, Cross FB)
  DLY_LPF,     // Echo Lowpass Frequency (Hi Cut)
  DLY_DRY,     // Echo Dry Amount (Mix)
  DLY_WET,     // Echo Wet Amount (Mix)
  DLY_SND,     // Echo Send Amount

  /* Reverb Signals                   -- monophonic */

  REV_SIZE,  // Reverb Size (Size)
  REV_FEED,  // Reverb Feedback Amount (Size)
  REV_BAL,   // Reverb Balance (Size)
  REV_PRE,   // Reverb Pre Delay (Pre Delay)
  REV_HPF,   // Reverb Highpass Frequency (Color)
  REV_LPF,   // Reverb Lowpass Frequency (Color)
  REV_CHO,   // Reverb Chorus (Chorus)
  REV_DRY,   // Reverb Dry Amount (Mix)
  REV_WET,   // Reverb Wet Amount (Mix)
  REV_SND,   // Reverb Send Amount

  /* Master Signals                   -- monophonic */

  MST_VOL,  // Master Volume (Volume)

  /* Unison Signals                   -- polyphonic */

  UN_PHS  // Unison Phase Offset
};
