#pragma once

/******************************************************************************/
/** @file       dsp_host_dual.h
    @date
    @version    1.7-0
    @author     M. Seeber
    @brief      new main engine container
    @todo
*******************************************************************************/

#include "c15_config.h"
#include "parameter_list.h"
#include "parameter_handle.h"
#include "pe_exponentiator.h"
#include "voice_allocation.h"

#include "ae_global_section.h"
#include "ae_poly_section.h"
#include "ae_mono_section.h"
#include "ae_fadepoint.h"

class dsp_host_dual
{
public:
    // public members
    float m_mainOut_R, m_mainOut_L;
    // constructor
    dsp_host_dual();
    // public methods
    void init(const uint32_t _samplerate, const uint32_t _polyphony);
    void onMidiMessage(const uint32_t _status, const uint32_t _data0, const uint32_t _data1);
    void render();
    void reset();
private:
    ParameterHandle<
        C15::Properties::Scale, C15::Descriptors::SmootherSection, C15::Descriptors::SmootherClock, C15::Descriptors::ParameterSignal,
        C15::Properties::LayerId, C15::Parameters::Hardware_Sources, C15::Parameters::Global_Parameters, C15::Parameters::Hardware_Amounts,
        C15::Parameters::Macro_Controls, C15::Parameters::Modulateable_Parameters, C15::Parameters::Unmodulateable_Parameters
    > m_params;
    // essential tools
    exponentiator m_convert;
    ClockHandle m_clock;
    TimeHandle m_time;
    uint32_t m_focus;
    // global dsp components
    GlobalSection m_global;
    // layered dsp components
    PolySection m_poly[2];
    MonoSection m_mono[2];
    ae_fadepoint m_fade[2];
    float scale(const C15::Properties::Scale _id, const float _scaleArg, float _value);
};
