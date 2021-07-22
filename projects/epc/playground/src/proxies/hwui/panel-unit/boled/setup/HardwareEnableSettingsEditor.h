#pragma once
#include "MenuEditor.h"
#include <playcontroller/playcontroller-defs.h>
#include <proxies/hwui/FrameBuffer.h>
#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/midi/RoutingSettings.h>

template <HW_SOURCE_IDS hw> class HardwareEnableSettingsEditor : public MenuEditor
{
 public:
  HardwareEnableSettingsEditor();
  void setPosition(const Rect& r) override;
  void drawBackground(FrameBuffer& fb) override;

 protected:
  void incSetting(int inc) override;
  const std::vector<Glib::ustring>& getDisplayStrings() const override;
  int getSelectedIndex() const override;
};

template <HW_SOURCE_IDS hw>
HardwareEnableSettingsEditor<hw>::HardwareEnableSettingsEditor()
    : MenuEditor()
{
  Application::get().getSettings()->getSetting<RoutingSettings>()->onChange(sigc::mem_fun(this, &HardwareEnableSettingsEditor<hw>::onSettingChanged));
}

template <HW_SOURCE_IDS hw> void HardwareEnableSettingsEditor<hw>::setPosition(const Rect& r)
{
  static const Rect menuEditorPosition(0, 16, 256, 96);
  MenuEditor::setPosition(menuEditorPosition);

  int y = 0;
  int h = 12;
  int w = menuEditorPosition.getWidth();
  int x = (menuEditorPosition.getWidth() - w) / 2;

  for(auto& c : getControls())
  {
    c->setPosition(Rect(x, y, w, h));
    y += h;
  }
}

template <HW_SOURCE_IDS hw> void HardwareEnableSettingsEditor<hw>::drawBackground(FrameBuffer& fb)
{
  fb.setColor(FrameBufferColors::C128);
  fb.fillRect(getPosition());
}

template <HW_SOURCE_IDS hw> void HardwareEnableSettingsEditor<hw>::incSetting(int inc)
{
}

template <HW_SOURCE_IDS hw>
const std::vector<Glib::ustring>& HardwareEnableSettingsEditor<hw>::getDisplayStrings() const
{
  static std::vector<Glib::ustring> sTest = { "Foo", "Bar" };
  return sTest;
}

template <HW_SOURCE_IDS hw> int HardwareEnableSettingsEditor<hw>::getSelectedIndex() const
{
  return 0;
}
