#include <proxies/hwui/TextCropper.h>
#include "ModuleCaption.h"
#include "Application.h"
#include <proxies/hwui/HWUI.h>
#include <parameters/mono-mode-parameters/ModulateableMonoParameter.h>
#include <parameters/mono-mode-parameters/UnmodulateableMonoParameter.h>
#include <groups/MonoGroup.h>
#include <parameters/mono-mode-parameters/MonoGlideTimeParameter.h>
#include <parameters/UnisonVoicesParameter.h>
#include <groups/GlobalParameterGroups.h>
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/Parameter.h"
#include "proxies/hwui/panel-unit/boled/BOLED.h"

ModuleCaption::ModuleCaption(const Rect &pos)
    : super(pos)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &ModuleCaption::onParameterSelected)));

  Application::get().getHWUI()->onCurrentVoiceGroupChanged(sigc::mem_fun(this, &ModuleCaption::onSelectionChanged));

  Application::get().getPresetManager()->getEditBuffer()->onSoundTypeChanged(
      sigc::mem_fun(this, &ModuleCaption::onSoundTypeChanged));
}

Label::StringAndSuffix ModuleCaption::shortenStringIfNeccessary(std::shared_ptr<Font> font,
                                                                const Label::StringAndSuffix &text) const
{
  return TextCropper::shortenStringIfNeccessary(font, text.text, getWidth());
}

void ModuleCaption::onParameterSelected(Parameter *newOne)
{
  updateText(newOne);
}

bool ModuleCaption::enableVoiceGroupSuffix() const
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto ebType = eb->getType();
  auto selected = eb->getSelected();

  return EditBuffer::isDualParameterForSoundType(selected, ebType);
};

void ModuleCaption::updateText(Parameter *newOne)
{
  if(newOne)
  {
    auto group = newOne->getParentGroup();
    auto groupName = group->getShortName();

    if(enableVoiceGroupSuffix())
    {
      auto sel = Application::get().getHWUI()->getCurrentVoiceGroup();
      auto suffix = std::string {};
      if(Application::get().getPresetManager()->getEditBuffer()->getType() != SoundType::Single)
        suffix = " " + toString(sel);
      setText(groupName + suffix);
    }
    else
      setText(groupName);
  }
}

void ModuleCaption::onSelectionChanged(VoiceGroup v)
{
  auto selected = Application::get().getPresetManager()->getEditBuffer()->getSelected();
  updateText(selected);
}

void ModuleCaption::onSoundTypeChanged()
{
  auto selected = Application::get().getPresetManager()->getEditBuffer()->getSelected();
  updateText(selected);
}

bool ModuleCaption::redraw(FrameBuffer &fb)
{
  const Rect &r = getPosition();

  fb.setColor(FrameBuffer::Colors::C128);
  fb.fillRect(r.getX(), r.getY(), r.getWidth(), r.getHeight());

  super::redraw(fb);
  return true;
}

void ModuleCaption::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C43);
}

std::shared_ptr<Font> ModuleCaption::getFont() const
{
  return Oleds::get().getFont("Emphase_8_Regular", getFontHeight());
}

int ModuleCaption::getFontHeight() const
{
  return 8;
}
