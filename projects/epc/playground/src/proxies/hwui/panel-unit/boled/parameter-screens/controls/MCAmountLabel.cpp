#include "MCAmountLabel.h"
#include <proxies/hwui/HWUI.h>
#include <device-settings/DebugLevel.h>
#include "Application.h"
#include "parameters/ModulateableParameter.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/scale-converters/LinearBipolar100PercentScaleConverter.h"
#include <sigc++/sigc++.h>
#include <proxies/hwui/FrameBuffer.h>

MCAmountLabel::MCAmountLabel(const Rect &rect)
    : super(rect)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &MCAmountLabel::onParameterSelected)), getHWUI()->getCurrentVoiceGroup());

  Application::get().getHWUI()->onModifiersChanged(sigc::hide(sigc::mem_fun(this, &MCAmountLabel::onModifiersChanged)));
}

MCAmountLabel::~MCAmountLabel()
{
}

void MCAmountLabel::onParameterSelected(Parameter *newParameter, SignalOrigin signalType)
{
  if(newParameter)
  {
    m_paramValueConnection.disconnect();
    m_paramValueConnection = newParameter->onParameterChanged(sigc::mem_fun(this, &MCAmountLabel::update));
  }
}

void MCAmountLabel::update(const Parameter *parameter)
{
  if(const auto *mp = dynamic_cast<const ModulateableParameter *>(parameter))
  {
    if(mp->getModulationSource() != MacroControls::NONE)
    {
      auto amount = mp->stringizeModulationAmount();
      if(isHighlight() && Application::get().getHWUI()->isModifierSet(ButtonModifier::FINE))
      {
        setText({ amount + " F", 2 });
      }
      else
      {
        setText(amount);
      }

      return;
    }
  }

  setText("");
}

void MCAmountLabel::setSuffixFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBufferColors::C103);
}

void MCAmountLabel::onModifiersChanged()
{
  update(Application::get().getPresetManager()->getEditBuffer()->getSelected(getHWUI()->getCurrentVoiceGroup()));
}
