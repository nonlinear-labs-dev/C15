#include "ParameterNotAvailableInSoundInfo.h"
#include "ParameterCarousel.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterLayout.h>
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <sigc++/adaptors/hide.h>
#include <proxies/hwui/controls/CenterAlignedLabel.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModuleCaption.h>
#include <proxies/hwui/controls/Button.h>
#include <proxies/hwui/Oleds.h>

namespace detail
{
  class BoldCenterAlignedLabel : public CenterAlignedLabel
  {
   public:
    using CenterAlignedLabel::CenterAlignedLabel;
    std::shared_ptr<Font> getFont() const override
    {
      return Oleds::get().getFont("Emphase-8-Bold", getFontHeight());
    }
  };
}

ParameterNotAvailableInSoundInfo::ParameterNotAvailableInSoundInfo(const Rect &r)
    : ControlWithChildren(r)
{
  auto hwui = Application::get().getHWUI();
  auto eb = Application::get().getPresetManager()->getEditBuffer();

  auto vg = getHWUI()->getCurrentVoiceGroup();

  m_parameterSelectionConnection
      = eb->onSelectionChanged(sigc::mem_fun(this, &ParameterNotAvailableInSoundInfo::onSelectionChanged), vg);

  m_soundTypeConnection
      = eb->onSoundTypeChanged(sigc::hide(sigc::mem_fun(this, &ParameterNotAvailableInSoundInfo::onSoundTypeChanged)));

  m_parameterNameLabel
      = addControl(new detail::BoldCenterAlignedLabel(eb->getSelected(vg)->getLongName(), { 0, 8, 128, 10 }));
  m_parameterNameLabel->setHighlight(true);
  addControl(new CenterAlignedLabel("Only available with", { 0, 20, 128, 10 }))->setHighlight(true);
  addControl(new CenterAlignedLabel("Layer Sounds", { 0, 32, 128, 10 }))->setHighlight(true);

  auto selected = eb->getSelected(hwui->getCurrentVoiceGroup());
  setVisible(selected->isDisabled());
}

void ParameterNotAvailableInSoundInfo::setBackgroundColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBufferColors::C43);
}

void ParameterNotAvailableInSoundInfo::onSelectionChanged(const Parameter *old, const Parameter *newParam,
                                                          SignalOrigin signalType)
{
  m_parameterNameLabel->setText({ newParam->getLongName(), 0 });
  setVisible(newParam->isDisabled());
}

void ParameterNotAvailableInSoundInfo::onSoundTypeChanged()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto current = eb->getSelected(getHWUI()->getCurrentVoiceGroup());
  const auto vis = current->isDisabled();
  m_parameterNameLabel->setText({ current->getLongName(), 0 });
  setVisible(vis);
}

void ParameterNotAvailableInSoundInfo::setVisible(bool b)
{
  for(auto &l : getControls<Label>())
  {
    l->setHighlight(true);
  }

  Control::setVisible(b);
}

int ParameterNotAvailableInSoundInfo::getZPriority() const
{
  return 1;
}
