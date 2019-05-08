#include "MacroControlMappingGroup.h"
#include "HardwareSourcesGroup.h"

#include "parameters/PhysicalControlParameter.h"
#include "parameters/ModulateableParameter.h"
#include "parameters/ModulationRoutingParameter.h"
#include "parameters/MacroControlParameter.h"
#include "parameters/scale-converters/LinearBipolar100PercentScaleConverter.h"

MacroControlMappingGroup::MacroControlMappingGroup(ParameterGroupSet *parent, ParameterGroup *hw, ParameterGroup *mc)
    : ParameterGroup(parent, "MCM", "HW Amount", "Hardware Amount", "Hardware Amount")
    , m_hw(hw)
    , m_mc(mc)
{
}

MacroControlMappingGroup::~MacroControlMappingGroup()
{
}

void MacroControlMappingGroup::init()
{
  auto mc_a = dynamic_cast<MacroControlParameter *>(m_mc->getParameterByID(243));
  auto mc_b = dynamic_cast<MacroControlParameter *>(m_mc->getParameterByID(244));
  auto mc_c = dynamic_cast<MacroControlParameter *>(m_mc->getParameterByID(245));
  auto mc_d = dynamic_cast<MacroControlParameter *>(m_mc->getParameterByID(246));

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

  appendParameter(new ModulationRoutingParameter(this, 255, pedal1, mc_a,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 256, pedal1, mc_b,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 257, pedal1, mc_c,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 258, pedal1, mc_d,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 260, pedal2, mc_a,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 261, pedal2, mc_b,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 262, pedal2, mc_c,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 263, pedal2, mc_d,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 265, pedal3, mc_a,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 266, pedal3, mc_b,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 267, pedal3, mc_c,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 268, pedal3, mc_d,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 270, pedal4, mc_a,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 271, pedal4, mc_b,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 272, pedal4, mc_c,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 273, pedal4, mc_d,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 275, pitchbend, mc_a,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 276, pitchbend, mc_b,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 277, pitchbend, mc_c,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 278, pitchbend, mc_d,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 280, aftertouch, mc_a,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 281, aftertouch, mc_b,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 282, aftertouch, mc_c,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 283, aftertouch, mc_d,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 285, ribbon1, mc_a,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 286, ribbon1, mc_b,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 287, ribbon1, mc_c,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 288, ribbon1, mc_d,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 290, ribbon2, mc_a,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 291, ribbon2, mc_b,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 292, ribbon2, mc_c,
                                                 ScaleConverter::get<LinearBipolar100PercentScaleConverter>()));

  appendParameter(new ModulationRoutingParameter(this, 293, ribbon2, mc_d,
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
