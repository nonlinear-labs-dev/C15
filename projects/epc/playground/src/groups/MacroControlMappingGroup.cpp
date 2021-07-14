#include "MacroControlMappingGroup.h"
#include "HardwareSourcesGroup.h"

#include "parameters/PhysicalControlParameter.h"
#include "parameters/ModulateableParameter.h"
#include "parameters/ModulationRoutingParameter.h"
#include "parameters/MacroControlParameter.h"
#include "parameters/scale-converters/LinearBipolar100PercentScaleConverter.h"

MacroControlMappingGroup::MacroControlMappingGroup(ParameterGroupSet *parent, ParameterGroup *hw, ParameterGroup *mc)
    : ParameterGroup(parent, { "MCM", VoiceGroup::Global }, "HW Amount", "Hardware Amount", "Hardware Amount")
    , m_mc(mc)
    , m_hw(hw)
{
}

MacroControlMappingGroup::~MacroControlMappingGroup() = default;

void MacroControlMappingGroup::init()
{
  auto mc_a = dynamic_cast<MacroControlParameter *>(m_mc->getParameterByID({ 243, VoiceGroup::Global }));
  auto mc_b = dynamic_cast<MacroControlParameter *>(m_mc->getParameterByID({ 244, VoiceGroup::Global }));
  auto mc_c = dynamic_cast<MacroControlParameter *>(m_mc->getParameterByID({ 245, VoiceGroup::Global }));
  auto mc_d = dynamic_cast<MacroControlParameter *>(m_mc->getParameterByID({ 246, VoiceGroup::Global }));
  auto mc_e = dynamic_cast<MacroControlParameter *>(m_mc->getParameterByID({ 369, VoiceGroup::Global }));
  auto mc_f = dynamic_cast<MacroControlParameter *>(m_mc->getParameterByID({ 371, VoiceGroup::Global }));

  auto pedal1
      = dynamic_cast<PhysicalControlParameter *>(m_hw->getParameterByID(HardwareSourcesGroup::getPedal1ParameterID()));
  auto pedal2
      = dynamic_cast<PhysicalControlParameter *>(m_hw->getParameterByID(HardwareSourcesGroup::getPedal2ParameterID()));
  auto pedal3
      = dynamic_cast<PhysicalControlParameter *>(m_hw->getParameterByID(HardwareSourcesGroup::getPedal3ParameterID()));
  auto pedal4
      = dynamic_cast<PhysicalControlParameter *>(m_hw->getParameterByID(HardwareSourcesGroup::getPedal4ParameterID()));
  auto pitchbend = dynamic_cast<PhysicalControlParameter *>(
      m_hw->getParameterByID(HardwareSourcesGroup::getPitchbendParameterID()));
  auto aftertouch = dynamic_cast<PhysicalControlParameter *>(
      m_hw->getParameterByID(HardwareSourcesGroup::getAftertouchParameterID()));
  auto ribbon1 = dynamic_cast<PhysicalControlParameter *>(
      m_hw->getParameterByID(HardwareSourcesGroup::getUpperRibbonParameterID()));
  auto ribbon2 = dynamic_cast<PhysicalControlParameter *>(
      m_hw->getParameterByID(HardwareSourcesGroup::getLowerRibbonParameterID()));

  appendParameter(new ModulationRoutingParameter(this, { 255, VoiceGroup::Global }, pedal1, mc_a,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 256, VoiceGroup::Global }, pedal1, mc_b,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 257, VoiceGroup::Global }, pedal1, mc_c,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 258, VoiceGroup::Global }, pedal1, mc_d,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 373, VoiceGroup::Global }, pedal1, mc_e,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 374, VoiceGroup::Global }, pedal1, mc_f,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 260, VoiceGroup::Global }, pedal2, mc_a,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 261, VoiceGroup::Global }, pedal2, mc_b,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 262, VoiceGroup::Global }, pedal2, mc_c,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 263, VoiceGroup::Global }, pedal2, mc_d,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 375, VoiceGroup::Global }, pedal2, mc_e,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 376, VoiceGroup::Global }, pedal2, mc_f,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 265, VoiceGroup::Global }, pedal3, mc_a,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 266, VoiceGroup::Global }, pedal3, mc_b,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 267, VoiceGroup::Global }, pedal3, mc_c,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 268, VoiceGroup::Global }, pedal3, mc_d,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 377, VoiceGroup::Global }, pedal3, mc_e,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 378, VoiceGroup::Global }, pedal3, mc_f,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 270, VoiceGroup::Global }, pedal4, mc_a,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 271, VoiceGroup::Global }, pedal4, mc_b,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 272, VoiceGroup::Global }, pedal4, mc_c,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 273, VoiceGroup::Global }, pedal4, mc_d,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 379, VoiceGroup::Global }, pedal4, mc_e,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 380, VoiceGroup::Global }, pedal4, mc_f,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 275, VoiceGroup::Global }, pitchbend, mc_a,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 276, VoiceGroup::Global }, pitchbend, mc_b,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 277, VoiceGroup::Global }, pitchbend, mc_c,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 278, VoiceGroup::Global }, pitchbend, mc_d,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 381, VoiceGroup::Global }, pitchbend, mc_e,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 382, VoiceGroup::Global }, pitchbend, mc_f,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 280, VoiceGroup::Global }, aftertouch, mc_a,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 281, VoiceGroup::Global }, aftertouch, mc_b,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 282, VoiceGroup::Global }, aftertouch, mc_c,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 283, VoiceGroup::Global }, aftertouch, mc_d,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 383, VoiceGroup::Global }, aftertouch, mc_e,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 384, VoiceGroup::Global }, aftertouch, mc_f,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 285, VoiceGroup::Global }, ribbon1, mc_a,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 286, VoiceGroup::Global }, ribbon1, mc_b,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 287, VoiceGroup::Global }, ribbon1, mc_c,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 288, VoiceGroup::Global }, ribbon1, mc_d,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 385, VoiceGroup::Global }, ribbon1, mc_e,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 386, VoiceGroup::Global }, ribbon1, mc_f,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 290, VoiceGroup::Global }, ribbon2, mc_a,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 291, VoiceGroup::Global }, ribbon2, mc_b,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 292, VoiceGroup::Global }, ribbon2, mc_c,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 293, VoiceGroup::Global }, ribbon2, mc_d,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 387, VoiceGroup::Global }, ribbon2, mc_e,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, { 388, VoiceGroup::Global }, ribbon2, mc_f,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));
}

MacroControlMappingGroup::tModRoutingParams
    MacroControlMappingGroup::getModulationRoutingParametersFor(const MacroControlParameter *mc)
{
  tModRoutingParams ret;

  for(auto param : getParameters())
    if(auto routingParam = dynamic_cast<ModulationRoutingParameter *>(param))
      if(mc == routingParam->getTargetParameter())
        ret.push_back(routingParam);

  return ret;
}

MacroControlMappingGroup::tModRoutingParams
    MacroControlMappingGroup::getModulationRoutingParametersFor(const PhysicalControlParameter *p)
{
  tModRoutingParams ret;

  for(auto param : getParameters())
    if(auto routingParam = dynamic_cast<ModulationRoutingParameter *>(param))
      if(p == routingParam->getSourceParameter())
        ret.push_back(routingParam);

  return ret;
}

const ModulationRoutingParameter *
    MacroControlMappingGroup::getModulationRoutingParameterFor(const PhysicalControlParameter *src,
                                                               const MacroControlParameter *mc)
{
  for(auto param : getParameters())
    if(auto routingParam = dynamic_cast<const ModulationRoutingParameter *>(param))
      if(src == routingParam->getSourceParameter() && mc == routingParam->getTargetParameter())
        return routingParam;

  return nullptr;
}

MacroControlMappingGroup::tPhysicalControlParameters MacroControlMappingGroup::getPhysicalControlParameters()
{
  tPhysicalControlParameters ret;

  for(auto param : getParameters())
    if(auto physicalParam = dynamic_cast<PhysicalControlParameter *>(param))
      ret.push_back(physicalParam);

  return ret;
}
