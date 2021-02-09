#include "SelectedMacroControlsHWSourceSlider.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/MacroControlParameter.h"
#include <parameters/PhysicalControlParameter.h>
#include <sigc++/sigc++.h>
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/HWUI.h>

SelectedMacroControlsHWSourceSlider::SelectedMacroControlsHWSourceSlider(const Rect &rect)
    : super(rect)
    , m_hwParamID(ParameterId::invalid())
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &SelectedMacroControlsHWSourceSlider::onParameterSelected)),
      getHWUI()->getCurrentVoiceGroup());
}

void SelectedMacroControlsHWSourceSlider::onParameterSelected(Parameter *newOne, SignalOrigin signalType)
{
  m_mcChanged.disconnect();
  m_mcChanged = newOne->onParameterChanged(sigc::mem_fun(this, &SelectedMacroControlsHWSourceSlider::onMCChanged));
}

void SelectedMacroControlsHWSourceSlider::onMCChanged(const Parameter *param)
{
  if(auto mc = dynamic_cast<const MacroControlParameter *>(param))
  {
    auto hwSourceID = mc->getUiSelectedHardwareSource();

    if(m_hwParamID != hwSourceID)
    {
      m_hwParamID = hwSourceID;

      if(hwSourceID.getNumber() > 0)
      {
        setParameter(Application::get().getPresetManager()->getEditBuffer()->findParameterByID(hwSourceID),
                     SignalOrigin::EXPLICIT);
      }
    }
  }
}

void SelectedMacroControlsHWSourceSlider::onParamValueChanged(const Parameter *param)
{
  Slider::onParamValueChanged(param);
  setDirty();
}

void SelectedMacroControlsHWSourceSlider::drawSliderBackground(FrameBuffer &fb)
{
  super::drawSliderBackground(fb);

  if(auto p = dynamic_cast<PhysicalControlParameter *>(getParameter()))
  {
    auto returnMode = p->getReturnMode();
    const auto &r = getPosition();
    const int centerMarkWidth = 4;

    switch(returnMode)
    {
      case ReturnMode::Center:
        fb.setColor(FrameBufferColors::C128);
        fb.fillRect(r.getCenter().getX() - centerMarkWidth / 2, r.getTop(), centerMarkWidth, r.getHeight());
        break;

      case ReturnMode::Zero:
        fb.setColor(FrameBufferColors::C128);
        fb.fillRect(r.getLeft(), r.getTop(), centerMarkWidth, r.getHeight());
        break;

      case ReturnMode::None:
      default:
        break;
    }
  }
}
