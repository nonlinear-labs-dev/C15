#include <Application.h>
#include <device-settings/DirectLoadSetting.h>
#include <device-settings/Settings.h>
#include <presets/Preset.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <presets/Bank.h>
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetLabel.h>

PresetLabel::PresetLabel(const Rect &pos)
    : super(pos)
{
}

PresetLabel::~PresetLabel()
{
}

bool PresetLabel::showsLoadedPreset() const
{
  return m_loaded;
}

bool PresetLabel::showsSelectedPreset() const
{
  return m_selected;
}

void PresetLabel::setText(const Glib::ustring &text, bool selected, bool loaded)
{
  super::setText(text);
  m_selected = selected;
  m_loaded = loaded;
  setDirty();
}

void PresetLabel::setFontColor(FrameBuffer &fb) const
{
  if(m_selected)
    fb.setColor(FrameBufferColors::C255);
  else
    fb.setColor(FrameBufferColors::C204);
}
