#pragma once

#include "ParameterGroup.h"

class ModulationRoutingParameter;
class PhysicalControlParameter;
class MacroControlParameter;

class HardwareSourcesGroup : public ParameterGroup
{
 public:
  HardwareSourcesGroup(ParameterGroupSet* parent);
  virtual ~HardwareSourcesGroup();

  typedef PhysicalControlParameter* tPhysicalControlParameter;
  typedef list<tPhysicalControlParameter> tPhysicalControlParameters;
  tPhysicalControlParameters getPhysicalControlParameters();

  constexpr static int getUpperRibbonParameterID()
  {
    return 284;
  }

  constexpr static int getLowerRibbonParameterID()
  {
    return 289;
  }

  constexpr static int getPedal1ParameterID()
  {
    return 254;
  }

  constexpr static int getPedal2ParameterID()
  {
    return 259;
  }

  constexpr static int getPedal3ParameterID()
  {
    return 264;
  }

  constexpr static int getPedal4ParameterID()
  {
    return 269;
  }

  constexpr static int getPitchbendParameterID()
  {
    return 274;
  }

  constexpr static int getAftertouchParameterID()
  {
    return 279;
  }

  virtual void init() override;
};
