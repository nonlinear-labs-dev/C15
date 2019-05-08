#pragma once

#include "ParameterGroup.h"

class HardwareSourcesGroup;
class ModulationRoutingParameter;
class PhysicalControlParameter;
class MacroControlParameter;

class MacroControlMappingGroup : public ParameterGroup
{
 public:
  MacroControlMappingGroup(ParameterGroupSet *parent, ParameterGroup *hw, ParameterGroup *mc);
  virtual ~MacroControlMappingGroup();

  typedef ModulationRoutingParameter *tModRoutingParam;
  typedef list<tModRoutingParam> tModRoutingParams;
  tModRoutingParams getModulationRoutingParametersFor(const MacroControlParameter *mc);
  tModRoutingParams getModulationRoutingParametersFor(const PhysicalControlParameter *src);
  const ModulationRoutingParameter *getModulationRoutingParameterFor(const PhysicalControlParameter *src,
                                                                     const MacroControlParameter *mc);

  typedef PhysicalControlParameter *tPhysicalControlParameter;
  typedef list<tPhysicalControlParameter> tPhysicalControlParameters;
  tPhysicalControlParameters getPhysicalControlParameters();

  void init();

 private:
  ParameterGroup *m_mc = nullptr;
  ParameterGroup *m_hw = nullptr;
};
