#include "SelectedParamsMacroControlSlider.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/ModulateableParameter.h"
#include "groups/MacroControlsGroup.h"
#include <sigc++/sigc++.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterLayout.h>
#include <proxies/hwui/HWUI.h>

SelectedParamsMacroControlSlider::SelectedParamsMacroControlSlider(const Rect &rect)
    : super(rect)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &SelectedParamsMacroControlSlider::setTargetParameter)),
      getHWUI()->getCurrentVoiceGroup());
}

SelectedParamsMacroControlSlider::~SelectedParamsMacroControlSlider()
{
}

void SelectedParamsMacroControlSlider::setTargetParameter(Parameter *param, SignalOrigin signalType)
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
      setParameter(nullptr, signalType);
    }
  }
}

void SelectedParamsMacroControlSlider::onTargetParamValueChanged(const Parameter *param)
{
  if(const auto *modP = dynamic_cast<const ModulateableParameter *>(param))
  {
    auto src = modP->getModulationSource();
    auto srcParamID = MacroControlsGroup::modSrcToParamId(src);
    setParameter(Application::get().getPresetManager()->getEditBuffer()->findParameterByID(srcParamID),
                 SignalOrigin::IMPLICIT);
  }
  else
  {
    setParameter(nullptr, SignalOrigin::IMPLICIT);
  }
}

void SelectedParamsMacroControlSlider::setParameter(Parameter *param, SignalOrigin signalType)
{
  Slider::setParameter(param, signalType);
  if(param)
  {
    setVisible(!param->isDisabled());
  }
}

bool SelectedParamsMacroControlSlider::isVisible() const
{
  return getParameter() && super::isVisible();
}
