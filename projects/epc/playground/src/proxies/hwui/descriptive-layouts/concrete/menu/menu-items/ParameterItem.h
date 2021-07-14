#pragma once
#include "EditorItem.h"

class Parameter;

class ParameterItem : public EditorItem
{
 public:
  ParameterItem(Parameter *parameter, const Rect &r);
  MenuOverlay *createOverlay() override;

 private:
  Parameter *const m_parameter;
};