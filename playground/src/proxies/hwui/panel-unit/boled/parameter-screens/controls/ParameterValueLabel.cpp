#include "ParameterValueLabel.h"
#include <parameters/Parameter.h>

ParameterValueLabel::ParameterValueLabel(Parameter *param, const Rect &pos)
    : Label(pos)
{
  param->onParameterChanged(mem_fun(this, &ParameterValueLabel::updateText));
}

void ParameterValueLabel::updateText(const Parameter *param)
{
  setText(param->getDisplayString());
}
