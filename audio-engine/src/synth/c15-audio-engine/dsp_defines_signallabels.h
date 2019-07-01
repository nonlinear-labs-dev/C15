/******************************************************************************/
/** @file       dsp_defines_signallabels.h
    @date       2018-08-21
    @version    1.0
    @author     Matthias Seeber & Anton Schmied
    @brief      Defines for the individual Signal Names (indices) in the great
                shared m_paramsignaldata[][] array
*******************************************************************************/

#pragma once

/* Envelope Signals                 -- polyphonic */

#define ENV_A_MAG       0           // Envelope A (magnitude component)
#define ENV_A_TMB       1           // Envelope A (timbre component)
#define ENV_B_MAG       2           // Envelope B (magnitude component)
#define ENV_B_TMB       3           // Envelope B (timbre component)
#define ENV_C_CLIP      4           // Envelope C (clipped)
#define ENV_C_UNCL      5           // Envelope C (unclipped)
#define ENV_G_SIG       6           // Gate Envelope

/* Oscillator A Signals             -- polyphonic */

#define OSC_A_FRQ       7           // Oscillator A Frequency (Pitch, Pitch KT, Pitch Env C Amount, Env C, Master Tune, Reference, Key Pitch)
#define OSC_A_FLUEC     8           // Oscillator A Fluctuation (Amount, Env C Amount, Env C)
#define OSC_A_PMSEA     9           // Oscillator A PM Self (Amount, Env A Amount, Env A)
#define OSC_A_PMSSH     10          // Oscillator A PM Self Shaper (Amount)
#define OSC_A_PMBEB     11          // Oscillator A PM B (Amount, Env B Amount, Env B)
#define OSC_A_PMBSH     12          // Oscillator A PM B Shaper (Amount)
#define OSC_A_PMFEC     13          // Oscillator A PM FB (Amount, Env C Amount, Env C)
#define OSC_A_PHS       14          // Oscillator A Phase (Offset)
#define OSC_A_CHI       15          // Oscillator A Chirp Frequency (Cutoff)

/* Shaper A Signals                 -- polyphonic */

#define SHP_A_DRVEA     16          // Shaper A Drive (Drive, Env A Amount, Env A)
#define SHP_A_FLD       17          // Shaper A Fold (Amount)
#define SHP_A_ASM       18          // Shaper A Asymetry (Amount)
#define SHP_A_MIX       19          // Shaper A Mix (Amount)
#define SHP_A_FBM       20          // Shaper A Feedback Mix (Amount)
#define SHP_A_FBEC      21          // Shaper A Feedback Env C (Amount, Env C, Gate)
#define SHP_A_RM        22          // Shaper A Ringmod (Amount)

/* Oscillator B Signals             -- polyphonic */

#define OSC_B_FRQ       23          // Oscillator B Frequency (Pitch, Pitch KT, Pitch Env C Amount, Env C, Master Tune, Reference, Key Pitch)
#define OSC_B_FLUEC     24          // Oscillator B Fluctuation (Amount, Env C Amount, Env C)
#define OSC_B_PMSEB     25          // Oscillator B PM Self (Amount, Env B Amount, Env B)
#define OSC_B_PMSSH     26          // Oscillator B PM Self Shaper (Amount)
#define OSC_B_PMAEA     27          // Oscillator B PM A (Amount, Env A Amount, Env A)
#define OSC_B_PMASH     28          // Oscillator B PM A Shaper (Amount)
#define OSC_B_PMFEC     29          // Oscillator B PM FB (Amount, Env C Amount, Env C)
#define OSC_B_PHS       30          // Oscillator B Phase (Offset)
#define OSC_B_CHI       31          // Oscillator B Chirp Frequency (Cutoff)

/* Shaper B Signals                 -- polyphonic */

#define SHP_B_DRVEB     32          // Shaper B Drive (Drive, Env B Amount, Env B)
#define SHP_B_FLD       33          // Shaper B Fold (Amount)
#define SHP_B_ASM       34          // Shaper B Asymetry (Amount)
#define SHP_B_MIX       35          // Shaper B Mix (Amount)
#define SHP_B_FBM       36          // Shaper B Feedback Mix (Amount)
#define SHP_B_FBEC      37          // Shaper B Feedback Env C (Amount, Env C, Gate)
#define SHP_B_RM        38          // Shaper B Ringmod (Amount)

/* Comb Filter Signals              -- polyphonic */

#define CMB_AB          39          // Comb AB Input Mix (Amount)
#define CMB_FRQ         40          // Comb Frequency (Pitch, Pitch KT, Master Tune, Reference, Key Pitch)
#define CMB_BYP         41          // Comb Bypass (Pitch)
#define CMB_FEC         42          // Comb Pitch Env C (Frequency Factor)
#define CMB_DEC         43          // Comb Decay (Decay, Decay KT, Decay Gate Amount, Gate, Master Tune, Key Pitch)
#define CMB_APF         44          // Comb Allpass Frequency (Tune, AP KT, AP Env C Amount, Env C, Master Tune, Key Pitch)
#define CMB_APR         45          // Comb Allpass Resonance (Amount)
#define CMB_LPF         46          // Comb Hi Cut Frequency (Hi Cut, Hi Cut KT, Hi Cut Env C, Env C, Master Tune, Key Pitch)
#define CMB_PM          47          // Comb PM (Amount)
#define CMB_PMAB        48          // Comb PM AB Mix (Amount)

/* State Variable Filter Signals    -- polyphonic */

#define SVF_AB          49          // SVF AB Input Mix (Amount)
#define SVF_CMIX        50          // SVF Comb Input Mix (Amount)
#define SVF_F1_CUT      51          // SVF Upper Filter Frequency (Cutoff, Cutoff KT, Cutoff Env C Amount, Env C, Spread, Master Tune, Reference, Key Pitch)
#define SVF_F2_CUT      52          // SVF Lower Filter Frequency (Cutoff, Cutoff KT, Cutoff Env C Amount, Env C, Spread, Master Tune, Reference, Key Pitch)
#define SVF_F1_FM       53          // SVF Upper Filter FM Amount (Cutoff, Cutoff KT, Cutoff Env C Amount, Env C, Spread, FM, Master Tune, Reference, Key Pitch)
#define SVF_F2_FM       54          // SVF Lower Filter FM Amount (Cutoff, Cutoff KT, Cutoff Env C Amount, Env C, Spread, FM, Master Tune, Reference, Key Pitch)
// <SVF: RES FIX>
#define SVF_RES         55          // OLD: SVF Resonance (Resonance, Res KT, Res Env C Amount, Env C, Master Tune, Key Pitch)
#define SVF_RES_DAMP    55          // FIX: SVF Damping Factor derived from Resonance (Res, ResKT, Res EnvC, EnvC, MasterTune, KeyPitch)
#define SVF_RES_FMAX    56          // FIX: SVF Maximum Frequency derived from Resonance (...)
// </SVF: RES FIX>
#define SVF_LBH_1       57          // SVF Upper LBH Mix (Amount)
#define SVF_LBH_2       58          // SVF Lower LBH Mix (Amount)
#define SVF_PAR_1       59          // SVF Parallel Amount 1 (Amount)
#define SVF_PAR_2       60          // SVF Parallel Amount 2 (Amount)
#define SVF_PAR_3       61          // SVF Parallel Amount 3 (Amount)
#define SVF_PAR_4       62          // SVF Parallel Amount 4 (Amount)
#define SVF_FMAB        63          // SVF FM AB Mix (Amount)

/* Feedback Mixer Signals           -- polyphonic */

#define FBM_CMB         64          // FB Mix Comb Mix (Amount)
#define FBM_SVF         65          // FB Mix SVF Mix (Amount)
#define FBM_FX          66          // FB Mix Effects Mix (Amount)
#define FBM_REV         67          // FB Mix Reverb Mix (Amount)
#define FBM_DRV         68          // FB Mix Drive (Amount)
#define FBM_FLD         69          // FB Mix Fold (Amount)
#define FBM_ASM         70          // FB Mix Asymetry (Amount)
#define FBM_LVL         71          // FB Mix Level (Level, Level KT, Master Tune, Key Pitch)
#define FBM_HPF         72          // FB Mix Highpass Frequency (Master Tune, Key Pitch)

/* Output Mixer Signals             -- polyphonic */

#define OUT_A_L         73          // Out Mix Branch A Left Level (Level, Pan, Key Pan)
#define OUT_A_R         74          // Out Mix Branch A Right Level (Level, Pan, Key Pan)
#define OUT_B_L         75          // Out Mix Branch B Left Level (Level, Pan, Key Pan)
#define OUT_B_R         76          // Out Mix Branch B Right Level (Level, Pan, Key Pan)
#define OUT_CMB_L       77          // Out Mix Comb Left Level (Level, Pan, Key Pan)
#define OUT_CMB_R       78          // Out Mix Comb Right Level (Level, Pan, Key Pan)
#define OUT_SVF_L       79          // Out Mix SVF Left Level (Level, Pan, Key Pan)
#define OUT_SVF_R       80          // Out Mix SVF Right Level (Level, Pan, Key Pan)
#define OUT_DRV         81          // Out Mix Drive (Amount)
#define OUT_FLD         82          // Out Mix Fold (Amount)
#define OUT_ASM         83          // Out Mix Asymetry (Amount)
#define OUT_LVL         84          // Out Mix Level (Level)

/* Cabinet Signals                  -- monophonic */

#define CAB_DRV         85          // Cabinet Drive (Amount)
#define CAB_FLD         86          // Cabinet Fold (Amount)
#define CAB_ASM         87          // Cabinet Asymetry (Amount)
#define CAB_PRESAT      88          // Cabinet Saturation Level (Tilt - pre Shaper factor)
#define CAB_SAT         89          // Cabinet Saturation Level (Tilt - post Shaper factor)
#define CAB_TILT        90          // Cabinet Tilt (Tilt)
#define CAB_LPF         91          // Cabinet Hi Cut (Cutoff)
#define CAB_HPF         92          // Cabinet Lo Cut (Cutoff)
#define CAB_DRY         93          // Cabinet Dry Amount (Mix)
#define CAB_WET         94          // Cabinet Wet Amount (Mix)

/* Gap Filter Signals               -- monophonic */

#define GAP_LFL         95          // Gap Filter Left Lowpass Frequency (Center, Stereo, Gap)
#define GAP_HFL         96          // Gap Filter Left Highpass Frequency (Center, Stereo, Gap)
#define GAP_LFR         97          // Gap Filter Right Lowpass Frequency (Center, Stereo, Gap)
#define GAP_HFR         98          // Gap Filter Right Highpass Frequency (Center, Stereo, Gap)
#define GAP_RES         99          // Gap Filter Resonance (Amount)
#define GAP_HPLP        100          // Gap Filter Highpass to Lowpass Amount (Balance, Mix)
#define GAP_INLP        101         // Gap Filter Input to Lowpass Amount (Balance, Mix)
#define GAP_HPOUT       102         // Gap Filter Highpass to Output Amount (Balance, Mix)
#define GAP_LPOUT       103         // Gap Filter Lowpass to Output Amount (Balance, Mix)
#define GAP_INOUT       104         // Gap Filter Input to Output Amount (Balance, Mix)

/* Flanger Signals                  -- monophonic */

#define FLA_TMOD        105         // Flanger Time Modulation (Amount)
#define FLA_LFO_L       106         // Flanger Left Modulation Signal (Phase, Rate, Envelope, Flanger LFO/Envelope)
#define FLA_LFO_R       107         // Flanger Right Modulation Signal (Phase, Rate, Envelope, Flanger LFO/Envelope)
#define FLA_TL          108         // Flanger Left Time (Time, Stereo)
#define FLA_TR          109         // Flanger Right Time (Time, Stereo)
#define FLA_FB_LOC      110         // Flanger Local Feedback Amount (Feedback, Cross FB)
#define FLA_FB_CR       111         // Flanger Cross Feedback Amount (Feedback, Cross FB)
#define FLA_LPF         112         // Flanger Lowpass Frequency (Hi Cut)
#define FLA_DRY         113         // Flanger Dry Amount (Mix)
#define FLA_WET         114         // Flanger Wet Amount (Mix)
#define FLA_APF_L       115         // Flanger Left Allpass Frequency (AP Mod, AP Tune, Flanger LFO/Envelope)
#define FLA_APF_R       116         // Flanger Right Allpass Frequency (AP Mod, AP Tune, Flanger LFO/Envelope)

/* Echo Signals                     -- monophonic */

#define DLY_TL          117         // Echo Left Time (Time, Stereo)
#define DLY_TR          118         // Echo Right Time (Time, Stereo)
#define DLY_FB_LOC      119         // Echo Local Feedback (Feedback, Cross FB)
#define DLY_FB_CR       120         // Echo Cross Feedback (Feedback, Cross FB)
#define DLY_LPF         121         // Echo Lowpass Frequency (Hi Cut)
#define DLY_DRY         122         // Echo Dry Amount (Mix)
#define DLY_WET         123         // Echo Wet Amount (Mix)
#define DLY_SND         124         // Echo Send Amount

/* Reverb Signals                   -- monophonic */

#define REV_SIZE        125         // Reverb Size (Size)
#define REV_FEED        126         // Reverb Feedback Amount (Size)
#define REV_BAL         127         // Reverb Balance (Size)
#define REV_PRE         128         // Reverb Pre Delay (Pre Delay)
#define REV_HPF         129         // Reverb Highpass Frequency (Color)
#define REV_LPF         130         // Reverb Lowpass Frequency (Color)
#define REV_CHO         131         // Reverb Chorus (Chorus)
#define REV_DRY         132         // Reverb Dry Amount (Mix)
#define REV_WET         133         // Reverb Wet Amount (Mix)
#define REV_SND         134         // Reverb Send Amount

/* Master Signals                   -- monophonic */

#define MST_VOL         135         // Master Volume (Volume)

/* Unison Signals                   -- polyphonic */

#define UN_PHS          136         // Unison Phase Offset
