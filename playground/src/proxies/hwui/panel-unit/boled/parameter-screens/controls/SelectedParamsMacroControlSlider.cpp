#include "SelectedParamsMacroControlSlider.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/ModulateableParameter.h"
#include "groups/MacroControlsGroup.h"

SelectedParamsMacroControlSlider::SelectedParamsMacroControlSlider(const Rect &rect)
    : super(rect)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &SelectedParamsMacroControlSlider::setTargetParameter)));
}

SelectedParamsMacroControlSlider::~SelectedParamsMacroControlSlider()
{
}

void SelectedParamsMacroControlSlider::setTargetParameter(Parameter *param)
{
  if(m_targetParameter != param)
  {
    m_targetParameter = param;

    if(m_targetParameter)
    {
      m_targetParamValueConnection.disconnect();
      m_targetParamValueConnection = m_targetParameter->onParameterChanged(
          sigc::mem_fun(this, &SelectedParamsMacroControlSlider::onTargetParamValueChanged));
    }
    else
    {
      setParameter(nullptr);
    }
  }
}

void SelectedParamsMacroControlSlider::onTargetParamValueChanged(const Parameter *param)
{
  if(const ModulateableParameter *modP = dynamic_cast<const ModulateableParameter *>(param))
  {
    auto src = modP->getModulationSource();
    uint16_t srcParamID = MacroControlsGroup::modSrcToParamID(src);

    if(auto pa = Application::get().getPresetManager()->getEditBuffer()->findParameterByID(srcParamID))
      setParameter(pa);
    else
      setParameter(nullptr);
  }
  else
  {
    setParameter(nullptr);
  }
}

bool SelectedParamsMacroControlSlider::isVisible() const
{
  return getParameter() && super::isVisible();
}
