#pragma once

#include <groups/ParameterGroup.h>
#include <tools/Throttler.h>

class ParameterGroupSet;

class ScaleGroup : public ParameterGroup
{
 public:
  ScaleGroup(ParameterGroupSet *parent);
  virtual ~ScaleGroup();

  void init();
  static int getScaleBaseParameterID();

 private:
  void onBaseKeyParameterChanged(const Parameter *baseKeyParameter);

  Throttler m_updateNames;
};
