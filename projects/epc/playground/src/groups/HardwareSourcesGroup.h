#pragma once

#include "ParameterGroup.h"

class ModulationRoutingParameter;
class PhysicalControlParameter;
class MacroControlParameter;

class HardwareSourcesGroup : public ParameterGroup
{
 public:
  explicit HardwareSourcesGroup(ParameterGroupSet* parent);
  ~HardwareSourcesGroup() override;

  typedef PhysicalControlParameter* tPhysicalControlParameter;
  typedef std::list<tPhysicalControlParameter> tPhysicalControlParameters;
  tPhysicalControlParameters getPhysicalControlParameters();

  static ParameterId getUpperRibbonParameterID()
  {
    return { 284, VoiceGroup::Global };
  }

  static ParameterId getLowerRibbonParameterID()
  {
    return { 289, VoiceGroup::Global };
  }

  static ParameterId getPedal1ParameterID()
  {
    return { 254, VoiceGroup::Global };
  }

  static ParameterId getPedal2ParameterID()
  {
    return { 259, VoiceGroup::Global };
  }

  static ParameterId getPedal3ParameterID()
  {
    return { 264, VoiceGroup::Global };
  }

  static ParameterId getPedal4ParameterID()
  {
    return { 269, VoiceGroup::Global };
  }

  static ParameterId getPitchbendParameterID()
  {
    return { 274, VoiceGroup::Global };
  }

  static ParameterId getAftertouchParameterID()
  {
    return { 279, VoiceGroup::Global };
  }

  virtual void init() override;
};
