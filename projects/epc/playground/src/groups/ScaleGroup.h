#pragma once

#include <groups/ParameterGroup.h>
#include <nltools/threading/Throttler.h>

class ParameterGroupSet;

class ScaleGroup : public ParameterGroup
{
 public:
  explicit ScaleGroup(ParameterGroupSet* parent);
  ~ScaleGroup() override;

  void init() override;
  bool isAnyOffsetChanged() const;
  static int getScaleBaseParameterNumber();

  static bool isScaleParameter(const ParameterId& id);
  static bool isScaleParameter(const Parameter* parameter);

 private:
  void onBaseKeyParameterChanged(const Parameter* baseKeyParameter);

  Throttler m_updateNames;
};
