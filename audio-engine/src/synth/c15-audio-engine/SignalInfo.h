#pragma once

#include "dsp_defines_signallabels.h"
#include <initializer_list>
#include <unordered_map>

namespace SignalInfo
{
  namespace algorithm
  {
    template <Signals... Rest> struct contains_t;

    template <Signals id> struct contains_t<id>
    {
      static constexpr bool value = false;
    };

    template <Signals id, Signals current, Signals... Rest> struct contains_t<id, current, Rest...>
    {
      static constexpr int value = id == current ? true : contains_t<id, Rest...>::value;
    };

    template <Signals... Rest> struct find_position_t;

    template <Signals id> struct find_position_t<id>
    {
      static constexpr int value = 0;
    };

    template <Signals id, Signals current, Signals... Rest> struct find_position_t<id, current, Rest...>
    {
      static constexpr int value = (id == current) ? 0 : 1 + find_position_t<id, Rest...>::value;
    };
  }

  template <Signals... IDs> struct SignalList
  {
    template <Signals id> using Contains = algorithm::contains_t<id, IDs...>;
    template <Signals id> using IndexMapper = algorithm::find_position_t<id, IDs...>;

    static constexpr auto size = sizeof...(IDs);

    static const std::unordered_map<Signals, int> s_map;

    static inline bool contains(Signals id)
    {
      return s_map.count(id) > 0;
    }

    static inline int mapToIndex(Signals id)
    {
      return s_map.at(id);
    }

    static std::unordered_map<Signals, int> createMap()
    {
      std::unordered_map<Signals, int> ret;

      int index = 0;
      std::initializer_list<Signals> ids{ IDs... };

      for(auto i : ids)
        ret[i] = index++;

      return ret;
    }
  };

  template <Signals... IDs>
  const std::unordered_map<Signals, int> SignalList<IDs...>::s_map = SignalList<IDs...>::createMap();

  using PolySignals
      = SignalList<Signals::ENV_A_MAG, Signals::ENV_A_TMB, Signals::ENV_B_MAG, Signals::ENV_B_TMB, Signals::ENV_C_CLIP,
                   Signals::ENV_C_UNCL, Signals::ENV_G_SIG, Signals::OSC_A_FRQ, Signals::OSC_A_FLUEC,
                   Signals::OSC_A_PMSEA, Signals::OSC_A_PMBEB, Signals::OSC_A_PMFEC, Signals::SHP_A_DRVEA,
                   Signals::SHP_A_FBEC, Signals::UN_PHS, Signals::SVF_F2_CUT, Signals::SVF_F1_FM, Signals::OSC_B_PMAEA,
                   Signals::OSC_B_FRQ, Signals::OSC_B_FLUEC, Signals::OSC_B_PMSEB, Signals::OSC_B_PMFEC,
                   Signals::SHP_B_DRVEB, Signals::SHP_B_FBEC, Signals::CMB_FRQ, Signals::CMB_BYP, Signals::CMB_FEC,
                   Signals::CMB_DEC, Signals::CMB_APF, Signals::CMB_LPF, Signals::SVF_F1_CUT, Signals::SVF_F2_FM,
                   Signals::SVF_RES, Signals::SVF_RES_DAMP, Signals::SVF_RES_FMAX, Signals::FBM_LVL, Signals::FBM_HPF,
                   Signals::OUT_A_L, Signals::OUT_A_R, Signals::OUT_B_L, Signals::OUT_B_R, Signals::OUT_CMB_L,
                   Signals::OUT_CMB_R, Signals::OUT_SVF_L, Signals::OUT_SVF_R>;

  using MonoSignals = SignalList<
      Signals::OSC_A_PMSSH, Signals::OSC_A_PMBSH, Signals::OSC_A_PHS, Signals::OSC_A_CHI, Signals::SHP_A_FLD,
      Signals::SHP_A_ASM, Signals::SHP_A_MIX, Signals::SHP_A_FBM, Signals::SHP_A_RM, Signals::OSC_B_PMSSH,
      Signals::OSC_B_PMASH, Signals::OSC_B_PHS, Signals::OSC_B_CHI, Signals::SHP_B_FLD, Signals::SHP_B_ASM,
      Signals::SHP_B_MIX, Signals::SHP_B_FBM, Signals::SHP_B_RM, Signals::CMB_AB, Signals::CMB_APR, Signals::CMB_PM,
      Signals::CMB_PMAB, Signals::SVF_AB, Signals::SVF_CMIX, Signals::SVF_LBH_1, Signals::SVF_LBH_2, Signals::SVF_PAR_1,
      Signals::SVF_PAR_2, Signals::SVF_PAR_3, Signals::SVF_PAR_4, Signals::SVF_FMAB, Signals::FBM_CMB, Signals::FBM_SVF,
      Signals::FBM_FX, Signals::FBM_REV, Signals::FBM_DRV, Signals::FBM_FLD, Signals::FBM_ASM, Signals::OUT_DRV,
      Signals::OUT_FLD, Signals::OUT_ASM, Signals::OUT_LVL, Signals::CAB_DRV, Signals::CAB_FLD, Signals::CAB_ASM,
      Signals::CAB_PRESAT, Signals::CAB_SAT, Signals::CAB_TILT, Signals::CAB_LPF, Signals::CAB_HPF, Signals::CAB_DRY,
      Signals::CAB_WET, Signals::GAP_LFL, Signals::GAP_HFL, Signals::GAP_LFR, Signals::GAP_HFR, Signals::GAP_RES,
      Signals::GAP_HPLP, Signals::GAP_INLP, Signals::GAP_HPOUT, Signals::GAP_LPOUT, Signals::GAP_INOUT,
      Signals::FLA_TMOD, Signals::FLA_LFO_L, Signals::FLA_LFO_R, Signals::FLA_TL, Signals::FLA_TR, Signals::FLA_FB_LOC,
      Signals::FLA_FB_CR, Signals::FLA_LPF, Signals::FLA_DRY, Signals::FLA_WET, Signals::FLA_APF_L, Signals::FLA_APF_R,
      Signals::DLY_TL, Signals::DLY_TR, Signals::DLY_FB_LOC, Signals::DLY_FB_CR, Signals::DLY_LPF, Signals::DLY_DRY,
      Signals::DLY_WET, Signals::DLY_SND, Signals::REV_SIZE, Signals::REV_FEED, Signals::REV_BAL, Signals::REV_PRE,
      Signals::REV_HPF, Signals::REV_LPF, Signals::REV_CHO, Signals::REV_DRY, Signals::REV_WET, Signals::REV_SND,
      Signals::MST_VOL>;
}
