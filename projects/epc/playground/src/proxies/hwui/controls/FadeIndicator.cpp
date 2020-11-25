#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/FrameBuffer.h>
#include <any>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <sigc++/adaptors/bind.h>
#include <parameter_declarations.h>
#include "FadeIndicator.h"

FadeIndicator::FadeIndicator(const Rect& r, VoiceGroup vg)
    : Control(r)
    , m_voiceGroup { vg }
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  eb->onChange(sigc::bind(sigc::mem_fun(this, &FadeIndicator::onEditBufferChanged), eb), true);
}

bool FadeIndicator::redraw(FrameBuffer& fb)
{
  if(isVisible())
  {
    auto y = getPosition().getTop();

    if(m_voiceGroup == VoiceGroup::II)
      y = getPosition().getBottom() + 1;

    auto updateY = [this](const auto x, auto& y) {
      if(m_voiceGroup == VoiceGroup::I)
      {
        if(x > getPosition().getX() + (getPosition().getWidth() / 2))
        {
          y++;
        }
      }
      else
      {
        if(x < getPosition().getX() + (getPosition().getWidth() / 2))
        {
          y--;
        }
      }
    };

    for(auto x = getPosition().getX(); x < getPosition().getX() + getWidth(); x++)
    {
      updateY(x, y);
      drawColumn(fb, x, y);
    }
  }
  return true;
}
void FadeIndicator::drawColumn(FrameBuffer& fb, int x, int y) const
{
  fb.setColor(FrameBufferColors::C179);
  fb.setPixel(x, y);
  fb.setColor(FrameBufferColors::C128);
  for(auto dy = y + 1; dy <= getPosition().getBottom(); dy++)
  {
    fb.setPixel(x, dy);
  }
}

void FadeIndicator::drawBackground(FrameBuffer& fb)
{
}

void FadeIndicator::onEditBufferChanged(const EditBuffer* eb)
{
  auto fadeActive = !eb->findParameterByID({ C15::PID::Voice_Grp_Fade_From, m_voiceGroup })->isFactoryDefaultLoaded();
  auto muteActive = !eb->findParameterByID({ C15::PID::Voice_Grp_Mute, m_voiceGroup })->isFactoryDefaultLoaded();
  setVisible(fadeActive && !muteActive);
}