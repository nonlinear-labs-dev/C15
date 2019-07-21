#pragma once

#include <groups/ParameterGroup.h>
#include <nltools/threading/Throttler.h>

class ParameterGroupSet;

class ScaleGroup : public ParameterGroup
{
 public:
  ScaleGroup(ParameterGroupSet *parent);
  virtual ~ScaleGroup();

  void init();
  bool isAnyOffsetChanged() const;
  static int getScaleBaseParameterID();

 private:
  void onBaseKeyParameterChanged(const Parameter *baseKeyParameter);

  Throttler m_updateNames;
};
