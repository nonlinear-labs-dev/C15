#include "ParameterValueLabel.h"
#include <parameters/Parameter.h>
#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <sigc++/sigc++.h>

ParameterValueLabel::ParameterValueLabel(const Parameter *param, const Rect &pos)
    : Label(pos)
{

  updateParameter(param);

  m_vgSelectionConnection = Application::get().getHWUI()->onCurrentVoiceGroupChanged(
      sigc::mem_fun(this, &ParameterValueLabel::updateVoiceGroup));
}

ParameterValueLabel::~ParameterValueLabel()
{
  m_connection.disconnect();
  m_vgSelectionConnection.disconnect();
}

void ParameterValueLabel::updateText(const Parameter *param)
{
  setText(StringAndSuffix { param->getDisplayString() });
}

void ParameterValueLabel::updateVoiceGroup(VoiceGroup newVoiceGroup)
{
  if(m_currentParameter)
  {
    if(m_currentParameter->getID().getVoiceGroup() != newVoiceGroup)
    {
      auto newSelection = Application::get().getPresetManager()->getEditBuffer()->findParameterByID(
          { m_currentParameter->getID().getNumber(), newVoiceGroup });
      if(newSelection)
      {
        updateParameter(newSelection);
        updateText(m_currentParameter);
      }
    }
  }
}

void ParameterValueLabel::updateParameter(const Parameter *param)
{
  m_connection.disconnect();

  if(param)
    m_connection = param->onParameterChanged(mem_fun(this, &ParameterValueLabel::updateText));

  m_currentParameter = param;
}
