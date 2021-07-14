#include <Application.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/controls/SelectedParameterValue.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/FrameBuffer.h>
#include <sigc++/sigc++.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterLayout.h>

SelectedParameterValue::SelectedParameterValue(const Rect &rect)
    : super(rect)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &SelectedParameterValue::onParameterSelected)),
      getHWUI()->getCurrentVoiceGroup());

  Application::get().getHWUI()->onModifiersChanged(sigc::mem_fun(this, &SelectedParameterValue::onModifiersChanged));

  Application::get().getHWUI()->onCurrentVoiceGroupChanged(
      sigc::mem_fun(this, &SelectedParameterValue::onVoiceGroupSelectionChanged));

  Application::get().getPresetManager()->getEditBuffer()->onSoundTypeChanged(
      sigc::hide(sigc::mem_fun(this, &SelectedParameterValue::onSoundTypeChanged)), false);
}

SelectedParameterValue::~SelectedParameterValue()
{
}

void SelectedParameterValue::onModifiersChanged(ButtonModifiers mods)
{
  onParamValueChanged(
      Application::get().getPresetManager()->getEditBuffer()->getSelected(getHWUI()->getCurrentVoiceGroup()));
}

void SelectedParameterValue::onParameterSelected(Parameter *parameter)
{
  m_paramValueConnection.disconnect();

  if(parameter)
  {
    m_paramValueConnection
        = parameter->onParameterChanged(sigc::mem_fun(this, &SelectedParameterValue::onParamValueChanged));

    setVisible(!parameter->isDisabled());
  }
}

void SelectedParameterValue::onParamValueChanged(const Parameter *param)
{
  setDirty();
}

bool SelectedParameterValue::redraw(FrameBuffer &fb)
{
  auto amount = Application::get()
                    .getPresetManager()
                    ->getEditBuffer()
                    ->getSelected(getHWUI()->getCurrentVoiceGroup())
                    ->getDisplayString();

  if(Application::get().getHWUI()->isModifierSet(ButtonModifier::FINE))
  {
    setText({ amount, " F" });
  }
  else
  {
    setText(StringAndSuffix { amount });
  }

  return super::redraw(fb);
}

void SelectedParameterValue::setSuffixFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBufferColors::C128);
}

void SelectedParameterValue::onVoiceGroupSelectionChanged(VoiceGroup v)
{
  setDirty();
}

void SelectedParameterValue::onSoundTypeChanged()
{
  auto selected
      = Application::get().getPresetManager()->getEditBuffer()->getSelected(getHWUI()->getCurrentVoiceGroup());
  setVisible(!selected->isDisabled());
}
