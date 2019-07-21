/******************************************************************************/
/** @file       pe_defines_lists.h
    @date       2018-03-13
    @version    1.0
    @author     Matthias Seeber
    @brief      predefined id list declaration (recall, key event)
                ..
    @todo
*******************************************************************************/

#pragma once

#include <stdint.h>
#include "pe_defines_config.h"
#include "pe_defines_labels.h"

// consecutive paramIds for MONO recall update (defining tcd order of destinations) - listId = 1
// NOTE:    - recall param id order by PLAYGROUND, list mechanism fragile/risky in this case -> discard

/* later: define "official" parameter order (playground, ...) in order to transmit preset lists -> also affects testconfig preset data! */
const Parameters paramIds_recall[lst_recall_length] = {

  // ENVELOPE A
  Parameters::P_EA_ATT, Parameters::P_EA_DEC1, Parameters::P_EA_BP, Parameters::P_EA_DEC2, Parameters::P_EA_SUS,
  Parameters::P_EA_REL, Parameters::P_EA_GAIN, Parameters::P_EA_LV, Parameters::P_EA_AV, Parameters::P_EA_D1V,
  Parameters::P_EA_D2V, Parameters::P_EA_RV, Parameters::P_EA_LKT, Parameters::P_EA_TKT, Parameters::P_EA_AC,
  Parameters::P_EA_SPL,

  // ENVELOPE B
  Parameters::P_EB_ATT, Parameters::P_EB_DEC1, Parameters::P_EB_BP, Parameters::P_EB_DEC2, Parameters::P_EB_SUS,
  Parameters::P_EB_REL, Parameters::P_EB_GAIN, Parameters::P_EB_LV, Parameters::P_EB_AV, Parameters::P_EB_D1V,
  Parameters::P_EB_D2V, Parameters::P_EB_RV, Parameters::P_EB_LKT, Parameters::P_EB_TKT, Parameters::P_EB_AC,
  Parameters::P_EB_SPL,

  // ENVELOPE C
  Parameters::P_EC_ATT, Parameters::P_EC_DEC1, Parameters::P_EC_BP, Parameters::P_EC_DEC2, Parameters::P_EC_SUS,
  Parameters::P_EC_REL, Parameters::P_EC_LV, Parameters::P_EC_AV, Parameters::P_EC_RV, Parameters::P_EC_LKT,
  Parameters::P_EC_TKT, Parameters::P_EC_AC, Parameters::P_EC_RH,

  // OSCILLATOR A
  Parameters::P_OA_P, Parameters::P_OA_PKT, Parameters::P_OA_PEC, Parameters::P_OA_F, Parameters::P_OA_FEC,
  Parameters::P_OA_PMS, Parameters::P_OA_PMSEA, Parameters::P_OA_PMSSH, Parameters::P_OA_PMB, Parameters::P_OA_PMBEB,
  Parameters::P_OA_PMBSH, Parameters::P_OA_PMF, Parameters::P_OA_PMFEC, Parameters::P_OA_PHS, Parameters::P_OA_CHI,

  // SHAPER A
  Parameters::P_SA_DRV, Parameters::P_SA_DEA, Parameters::P_SA_FLD, Parameters::P_SA_ASM, Parameters::P_SA_MIX,
  Parameters::P_SA_FBM, Parameters::P_SA_FBEC, Parameters::P_SA_RM,

  // OSCILLATOR B
  Parameters::P_OB_P, Parameters::P_OB_PKT, Parameters::P_OB_PEC, Parameters::P_OB_F, Parameters::P_OB_FEC,
  Parameters::P_OB_PMS, Parameters::P_OB_PMSEB, Parameters::P_OB_PMSSH, Parameters::P_OB_PMA, Parameters::P_OB_PMAEA,
  Parameters::P_OB_PMASH, Parameters::P_OB_PMF, Parameters::P_OB_PMFEC, Parameters::P_OB_PHS, Parameters::P_OB_CHI,

  // SHAPER B
  Parameters::P_SB_DRV, Parameters::P_SB_DEB, Parameters::P_SB_FLD, Parameters::P_SB_ASM, Parameters::P_SB_MIX,
  Parameters::P_SB_FBM, Parameters::P_SB_FBEC, Parameters::P_SB_RM,

  // COMB FILTER
  Parameters::P_CMB_AB, Parameters::P_CMB_P, Parameters::P_CMB_PKT, Parameters::P_CMB_PEC, Parameters::P_CMB_D,
  Parameters::P_CMB_DKT, Parameters::P_CMB_DG, Parameters::P_CMB_APT, Parameters::P_CMB_APKT, Parameters::P_CMB_APEC,
  Parameters::P_CMB_APR, Parameters::P_CMB_LP, Parameters::P_CMB_LPKT, Parameters::P_CMB_LPEC, Parameters::P_CMB_PM,
  Parameters::P_CMB_PMAB,

  // STATE VARIABLE FILTER
  Parameters::P_SVF_AB, Parameters::P_SVF_CMB, Parameters::P_SVF_CUT, Parameters::P_SVF_CKT, Parameters::P_SVF_CEC,
  Parameters::P_SVF_SPR, Parameters::P_SVF_FM, Parameters::P_SVF_RES, Parameters::P_SVF_RKT, Parameters::P_SVF_REC,
  Parameters::P_SVF_LBH, Parameters::P_SVF_PAR, Parameters::P_SVF_FMAB,

  // FEEDBACK MIXER
  Parameters::P_FBM_CMB, Parameters::P_FBM_SVF, Parameters::P_FBM_FX, Parameters::P_FBM_REV, Parameters::P_FBM_DRV,
  Parameters::P_FBM_FLD, Parameters::P_FBM_ASM, Parameters::P_FBM_LKT, Parameters::P_FBM_LVL,

  // OUTPUT MIXER
  Parameters::P_OM_AL, Parameters::P_OM_AP, Parameters::P_OM_BL, Parameters::P_OM_BP, Parameters::P_OM_CL,
  Parameters::P_OM_CP, Parameters::P_OM_SL, Parameters::P_OM_SP, Parameters::P_OM_DRV, Parameters::P_OM_FLD,
  Parameters::P_OM_ASM, Parameters::P_OM_LVL, Parameters::P_OM_KP,

  // CABINET
  Parameters::P_CAB_DRV, Parameters::P_CAB_FLD, Parameters::P_CAB_ASM, Parameters::P_CAB_TILT, Parameters::P_CAB_LPF,
  Parameters::P_CAB_HPF, Parameters::P_CAB_LVL, Parameters::P_CAB_MIX,

  // GAP FILTER
  Parameters::P_GAP_CNT, Parameters::P_GAP_STE, Parameters::P_GAP_GAP, Parameters::P_GAP_RES, Parameters::P_GAP_BAL,
  Parameters::P_GAP_MIX,

  // FLANGER
  Parameters::P_FLA_TMOD, Parameters::P_FLA_PHS, Parameters::P_FLA_RTE, Parameters::P_FLA_TIME, Parameters::P_FLA_STE,
  Parameters::P_FLA_FB, Parameters::P_FLA_CFB, Parameters::P_FLA_LPF, Parameters::P_FLA_MIX, Parameters::P_FLA_ENV,
  Parameters::P_FLA_APM, Parameters::P_FLA_APT,

  // ECHO
  Parameters::P_DLY_TIME, Parameters::P_DLY_STE, Parameters::P_DLY_FB, Parameters::P_DLY_CFB, Parameters::P_DLY_LPF,
  Parameters::P_DLY_MIX, Parameters::P_DLY_SND,

  // REVERB
  Parameters::P_REV_SIZE, Parameters::P_REV_PRE, Parameters::P_REV_COL, Parameters::P_REV_CHO, Parameters::P_REV_MIX,
  Parameters::P_REV_SND,

  // MASTER
  Parameters::P_MA_V, Parameters::P_MA_T,

  // UNISON
  Parameters::P_UN_V, Parameters::P_UN_DET, Parameters::P_UN_PHS, Parameters::P_UN_PAN

};

// consecutive paramIds for POLY key event update (defining tcd order of destinations) - listId = 2
// NOTE:    - keyEvent param id order by LPC, list mechanism usable -> keep

#if test_milestone == 150

const Parameters paramIds_keyEvent[lst_keyEvent_length]
    = { Parameters::P_KEY_PH, Parameters::P_KEY_NP, Parameters::P_KEY_VP, Parameters::P_KEY_VS };

#elif test_milestone == 155

const Parameters paramIds_keyEvent[lst_keyEvent_length]
    = { Parameters::P_KEY_BP, Parameters::P_KEY_IDX, Parameters::P_KEY_STL };

#elif test_milestone == 156

const Parameters paramIds_keyEvent[lst_keyEvent_length] = { Parameters::P_KEY_BP };

#endif
