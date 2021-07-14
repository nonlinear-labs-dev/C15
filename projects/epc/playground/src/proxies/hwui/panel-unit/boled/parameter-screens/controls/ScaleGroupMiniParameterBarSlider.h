#pragma once

#include "MiniParameterBarSlider.h"

class ScaleGroupMiniParameterBarSlider : public MiniParameterBarSlider
{
  using super = MiniParameterBarSlider;

 public:
  ScaleGroupMiniParameterBarSlider(Parameter *param, const Rect &rect);
  ~ScaleGroupMiniParameterBarSlider() override;

  tDisplayValue getDrawValue() const override;

 private:
  sigc::connection m_connection;
};
