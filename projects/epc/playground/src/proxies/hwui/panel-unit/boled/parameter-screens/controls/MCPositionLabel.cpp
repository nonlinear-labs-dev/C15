#include "MCPositionLabel.h"
#include <proxies/hwui/HWUI.h>
#include "Application.h"
#include "parameters/ModulateableParameter.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "groups/MacroControlsGroup.h"
#include <proxies/hwui/FrameBuffer.h>
#include <sigc++/sigc++.h>

MCPositionLabel::MCPositionLabel(const Rect &rect)
    : super(rect)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &MCPositionLabel::onParameterSelected)), getHWUI()->getCurrentVoiceGroup());

  Application::get().getHWUI()->onModifiersChanged(
      sigc::hide(sigc::mem_fun(this, &MCPositionLabel::onModifiersChanged)));
}

MCPositionLabel::~MCPositionLabel()
{
}

void MCPositionLabel::onParameterSelected(Parameter *newParameter, SignalOrigin signalType)
{
  if(newParameter)
  {
    m_paramValueConnection.disconnect();
    m_paramValueConnection = newParameter->onParameterChanged(sigc::mem_fun(this, &MCPositionLabel::updateTarget));
  }
}

void MCPositionLabel::updateTarget(const Parameter *parameter)
{
  ensureDisconnectedModulationSourceIfApplicable(parameter);

  if(const auto *modP = dynamic_cast<const ModulateableParameter *>(parameter))
  {
    auto src = modP->getModulationSource();
    auto srcParamID = MacroControlsGroup::modSrcToParamId(src);

    if(auto pa = Application::get().getPresetManager()->getEditBuffer()->findParameterByID(srcParamID))
    {
      m_mcValueConnection.disconnect();
      m_mcValueConnection = pa->onParameterChanged(sigc::mem_fun(this, &MCPositionLabel::updateSource));
      return;
    }
  }

  setText("");
}

void MCPositionLabel::updateSource(const Parameter *parameter)
{
  ensureDisconnectedModulationSourceIfApplicable(parameter);

  if(parameter)
  {
    auto t = parameter->getDisplayString();

    if(isHighlight() && Application::get().getHWUI()->isModifierSet(ButtonModifier::FINE))
    {
      setText({ t, " F" });
    }
    else
    {
      setText(t);
    }
  }
  else
  {
    setText("");
  }
}
void MCPositionLabel::setSuffixFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBufferColors::C103);
}

void MCPositionLabel::onModifiersChanged()
{
  onParameterSelected(
      Application::get().getPresetManager()->getEditBuffer()->getSelected(getHWUI()->getCurrentVoiceGroup()),
      SignalOrigin::EXPLICIT);
}

void MCPositionLabel::ensureDisconnectedModulationSourceIfApplicable(const Parameter *parameter)
{
  if(auto modP = dynamic_cast<const ModulateableParameter *>(parameter))
  {
    if(modP->getModulationSource() == MacroControls::NONE)
    {
      m_mcValueConnection.disconnect();
      setText("");
    }
  }
}
