#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MiniParameterBarSlider.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MiniParameterDotSlider.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ScaleGroupMiniParameterBarSlider.h>
#include <groups/ScaleGroup.h>
#include "MiniParameter.h"
#include "parameters/Parameter.h"
#include "MiniParameterLabel.h"

MiniParameter::MiniParameter(Parameter *param, const Rect &pos)
    : super(pos)
    , m_param(param)
{
  m_label = addControl(new MiniParameterLabel(param, Rect(0, 0, 56, 9)));

  if(param->getID().getNumber() == ScaleGroup::getScaleBaseParameterNumber())
  {
    addControl(new ScaleGroupMiniParameterBarSlider(param, Rect(0, 10, 56, 2)));
  }
  else
  {
    switch(param->getVisualizationStyle())
    {

      case Parameter::VisualizationStyle::Bar:
      case Parameter::VisualizationStyle::BarFromRight:
        addControl(new MiniParameterBarSlider(param, Rect(0, 10, 56, 2)));
        break;
      case Parameter::VisualizationStyle::Dot:
        addControl(new MiniParameterDotSlider(param, Rect(0, 10, 56, 2)));
        break;
    }
  }
}

MiniParameter::~MiniParameter()
{
}

Parameter *MiniParameter::getParameter()
{
  return m_param;
}

const Parameter *MiniParameter::getParameter() const
{
  return m_param;
}

void MiniParameter::setSelected(bool selected)
{
  m_label->setSelected(selected);
}

bool MiniParameter::isSelected() const
{
  return m_label->isSelected();
}
