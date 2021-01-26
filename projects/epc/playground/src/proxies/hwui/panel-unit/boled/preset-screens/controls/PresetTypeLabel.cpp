#include "PresetTypeLabel.h"
#include <proxies/hwui/FrameBuffer.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <device-settings/Settings.h>
#include <device-settings/DirectLoadSetting.h>
#include <proxies/hwui/HWUI.h>
#include <memory>
#include <proxies/hwui/panel-unit/boled/preset-screens/PresetManagerLayout.h>

PresetTypeLabel::PresetTypeLabel(const Rect &pos)
    : Control(pos)
{
  m_editbufferConnection = Application::get().getPresetManager()->getEditBuffer()->onChange(
      sigc::mem_fun(this, &PresetTypeLabel::onEditBufferChanged));

  m_voiceGroupChanged = Application::get().getHWUI()->onCurrentVoiceGroupChanged(
      sigc::mem_fun(this, &PresetTypeLabel::onVoiceGroupChanged));
}

PresetTypeLabel::~PresetTypeLabel()
{
  m_editbufferConnection.disconnect();
  m_voiceGroupChanged.disconnect();
}

void PresetTypeLabel::onEditBufferChanged()
{
  if(selectedPreset)
    update(selectedPreset);
}

void PresetTypeLabel::onVoiceGroupChanged(const VoiceGroup &vg)
{
  if(selectedPreset)
    update(selectedPreset);
}

bool PresetTypeLabel::redraw(FrameBuffer &fb)
{

  drawBackground(fb);

  if(m_currentControl)
  {
    return m_currentControl->redraw(fb);
  }
  return false;
}

void PresetTypeLabel::update(const Preset *newSelection)
{
  auto position = getPosition();
  selectedPreset = newSelection;
  auto isDualEditBuffer = Application::get().getPresetManager()->getEditBuffer()->isDual();

  if(Application::get().getHWUI()->isInLoadToPart() && isDualEditBuffer)
  {
    auto pos = Rect { position.getLeft(), position.getTop(), 10, 10 };
    m_currentControl = std::make_unique<DualPresetTypeLabel>(pos);
    auto dualLabel = dynamic_cast<DualPresetTypeLabel *>(m_currentControl.get());
    dualLabel->update(newSelection);
  }
  else
  {
    auto pos = Rect { position.getLeft(), position.getTop(), 10, 14 };
    m_currentControl = std::make_unique<SinglePresetTypeLabel>(pos);
    auto singleLabel = dynamic_cast<SinglePresetTypeLabel *>(m_currentControl.get());
    singleLabel->update(newSelection);
  }
}

void PresetTypeLabel::drawBackground(FrameBuffer &fb)
{

  auto hwui = Application::get().getHWUI();

  if(selectedPreset
     && Application::get().getPresetManager()->getEditBuffer()->getUUIDOfLastLoadedPreset()
         == selectedPreset->getUuid())
  {
    fb.setColor(FrameBufferColors::C103);
    auto pos = getPosition();

    bool selected = false;

    if(hwui->isInLoadToPart())
    {
      auto currentVGFocus = hwui->getCurrentVoiceGroup();
      if(auto selection = hwui->getPresetPartSelection(currentVGFocus))
      {
        selected = selection->m_preset == selectedPreset;
      }
    }
    else
    {
      selected = Application::get().getPresetManager()->getSelectedPreset() == selectedPreset;
    }

    if(selected)
    {
      pos.setTop(pos.getTop() + 2);
      pos.setHeight(pos.getHeight() - 4);
    }
    else
    {
      pos.setTop(pos.getTop() + 1);
      pos.setHeight(pos.getHeight() - 2);
    }
    fb.fillRect(pos);
  }
}

std::string typeToString(const SoundType &type)
{
  switch(type)
  {
    case SoundType::Single:
    case SoundType::Invalid:
      return "";
    case SoundType::Layer:
      return "\uE0F5";
    case SoundType::Split:
      return "\uE0F2";
  }
  return {};
}

SinglePresetTypeLabel::SinglePresetTypeLabel(const Rect &r)
    : PresetLabel(r)
{
}

int SinglePresetTypeLabel::getXOffset() const
{
  return 0;
}

void SinglePresetTypeLabel::update(const Preset *newPreset)
{
  if(newPreset)
  {
    auto type = newPreset->getType();
    auto loaded
        = Application::get().getPresetManager()->getEditBuffer()->getUUIDOfLastLoadedPreset() == newPreset->getUuid();

    auto selected = Application::get().getPresetManager()->getSelectedPreset() == newPreset;

    setText(typeToString(type), selected, loaded);
  }
}
Font::Justification SinglePresetTypeLabel::getJustification() const
{
  return Font::Justification::Left;
}

DualPresetTypeLabel::DualPresetTypeLabel(const Rect &r)
    : Control(r)
{
}

bool DualPresetTypeLabel::redraw(FrameBuffer &fb)
{
  if(!hasPreset)
  {
    fb.setColor(FrameBufferColors::Transparent);
    fb.fillRect(getPosition());
    return true;
  }
  else
  {
    auto offset = fb.offset(Point { 2, 0 });
    switch(m_presetType)
    {
      case SoundType::Single:
        return drawSingle(fb);
      case SoundType::Layer:
        return drawLayer(fb);
      case SoundType::Split:
        return drawSplit(fb);
      default:
      case SoundType::Invalid:
        return false;
    }
  }
}

bool DualPresetTypeLabel::drawLayer(FrameBuffer &buffer)
{
  auto bgRect = getPosition();
  bgRect.setWidth(12);
  bgRect.setHeight(15);
  buffer.setColor(FrameBufferColors::C43);
  buffer.fillRect(bgRect);

  buffer.setColor(m_inidicateI ? loadedColor : defaultColor);
  buffer.fillRect(getPosition().getX() + 2, getPosition().getY() + 3, 10, 4);
  if(m_selectedI)
  {
    buffer.setColor(selectColor);
    buffer.drawRect(getPosition().getX() + 2, getPosition().getY() + 3, 10, 4);
  }

  buffer.setColor(m_inidicateII ? loadedColor : defaultColor);
  buffer.fillRect(getPosition().getX() + 2, getPosition().getY() + 9, 10, 4);
  if(m_selectedII)
  {
    buffer.setColor(selectColor);
    buffer.drawRect(getPosition().getX() + 2, getPosition().getY() + 9, 10, 4);
  }
  return true;
}

bool DualPresetTypeLabel::drawSplit(FrameBuffer &buffer)
{
  auto bgRect = getPosition();
  bgRect.setWidth(12);
  bgRect.setHeight(15);
  buffer.setColor(FrameBufferColors::C43);
  buffer.fillRect(bgRect);

  buffer.setColor(m_inidicateI ? loadedColor : defaultColor);
  buffer.fillRect(getPosition().getX() + 2, getPosition().getY() + 3, 4, 10);
  if(m_selectedI)
  {
    buffer.setColor(selectColor);
    buffer.drawRect(getPosition().getX() + 2, getPosition().getY() + 3, 4, 10);
  }

  buffer.setColor(m_inidicateII ? loadedColor : defaultColor);
  buffer.fillRect(getPosition().getX() + 8, getPosition().getY() + 3, 4, 10);
  if(m_selectedII)
  {
    buffer.setColor(selectColor);
    buffer.drawRect(getPosition().getX() + 8, getPosition().getY() + 3, 4, 10);
  }
  return true;
}

bool DualPresetTypeLabel::drawSingle(FrameBuffer &buffer)
{
  buffer.setColor(m_inidicateI ? loadedColor : defaultColor);
  buffer.fillRect(getPosition().getX() + 2, getPosition().getY() + 3, 10, 10);
  if(m_selectedI)
  {
    buffer.setColor(selectColor);
    buffer.drawRect(getPosition().getX() + 2, getPosition().getY() + 3, 10, 10);
  }

  return true;
}

void DualPresetTypeLabel::update(const Preset *selected)
{
  hasPreset = selected != nullptr;

  if(selected)
  {
    auto hwui = Application::get().getHWUI();
    auto currentVGFocus = hwui->getCurrentVoiceGroup();
    const auto origin = Application::get().getPresetManager()->getEditBuffer()->getPartOrigin(currentVGFocus);

    auto selection = hwui->getPresetPartSelection(currentVGFocus);

    const auto &presetUUID = selected->getUuid();

    m_presetType = selected->getType();

    m_anySelected = selection->m_preset->getUuid() == selected->getUuid();
    m_selectedI = m_anySelected && selection->m_voiceGroup == VoiceGroup::I;
    m_selectedII = m_anySelected && selection->m_voiceGroup == VoiceGroup::II;

    m_presetLoaded = origin.presetUUID == presetUUID;
    m_inidicateI = m_presetLoaded && origin.sourceGroup == VoiceGroup::I;
    m_inidicateII = m_presetLoaded && origin.sourceGroup == VoiceGroup::II;

    setDirty();
  }
}
