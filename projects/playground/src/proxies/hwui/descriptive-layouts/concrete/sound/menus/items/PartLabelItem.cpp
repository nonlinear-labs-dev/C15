#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/RenameVoiceGroupLayout.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/SoundEditInfoLayout.h>

#include "PartLabelItem.h"

inline EditBuffer* getEditBuffer()
{
  return Application::get().getPresetManager()->getEditBuffer();
}

VoiceGroup currentVoiceGroup()
{
  return Application::get().getHWUI()->getCurrentVoiceGroup();
}

Glib::ustring getPartLabel(VoiceGroup vg)
{
  auto eb = getEditBuffer();
  return eb->getVoiceGroupName(vg);
}

PartLabelItem::PartLabelItem(const Rect& r)
    : BasicItemWithValueText("Part Label", getPartLabel(currentVoiceGroup()), r)
{
}

void PartLabelItem::doAction()
{
  auto hwui = Application::get().getHWUI();
  hwui->getPanelUnit().getEditPanel().getBoled().reset(new RenameVoiceGroupLayout(currentVoiceGroup()));
}

Control * PartLabelItem::createInfo(){
  return new SoundEditInfoLayout(SetupInfoEntries::PartLabel);
}
