#include "PresetTypeLabel.h"
#include <proxies/hwui/FrameBuffer.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <device-settings/Settings.h>
#include <device-settings/DirectLoadSetting.h>
#include <proxies/hwui/HWUI.h>
#include <memory>
#include <proxies/hwui/HWUIHelper.h>
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

  if(HWUIHelper::isLoadToPartActive() && isDualEditBuffer)
  {
    m_currentControl = std::make_unique<DualPresetTypeLabel>(
        Rect { position.getLeft(), position.getTop(), 10, 10 });
    auto dualLabel = dynamic_cast<DualPresetTypeLabel *>(m_currentControl.get());
    dualLabel->update(newSelection);
  }
  else
  {
    m_currentControl = std::make_unique<SinglePresetTypeLabel>(position);
    auto singleLabel = dynamic_cast<SinglePresetTypeLabel *>(m_currentControl.get());
    singleLabel->update(newSelection);
  }
}

void PresetTypeLabel::drawBackground(FrameBuffer &fb)
{

  if(selectedPreset
     && Application::get().getPresetManager()->getEditBuffer()->getUUIDOfLastLoadedPreset()
         == selectedPreset->getUuid())
  {
    fb.setColor(FrameBufferColors::C103);
    auto pos = getPosition();

    bool selected = false;

    if(HWUIHelper::isLoadToPartActive())
    {
      auto currentVGFocus = Application::get().getHWUI()->getCurrentVoiceGroup();
      auto currentLayout = Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().getLayout().get();
      if(auto presetManagerLayout = dynamic_cast<PresetManagerLayout *>(currentLayout))
      {
        if(auto selection = presetManagerLayout->getPresetPartSelection(currentVGFocus))
        {
          selected = selection->m_preset == selectedPreset;
        }
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
      return "\uE0F3";
    case SoundType::Split:
      return "\uE0F0";
  }
  return {};
}

SinglePresetTypeLabel::SinglePresetTypeLabel(const Rect &r)
    : PresetLabel(r)
{
}

int SinglePresetTypeLabel::getXOffset() const
{
  return 4;
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
      case SoundType::Invalid:
        return false;
    }
  }
  return false;
}

bool DualPresetTypeLabel::drawLayer(FrameBuffer &buffer)
{
  auto bgRect = getPosition();
  bgRect.setWidth(14);
  bgRect.setHeight(15);
  buffer.setColor(FrameBufferColors::C43);
  buffer.fillRect(bgRect);

  buffer.setColor(m_inidicateI ? FrameBufferColors::C179 : FrameBufferColors::C128);
  buffer.fillRect(getPosition().getX() + 2, getPosition().getY() + 3, 10, 4);
  if(m_selectedI)
  {
    buffer.setColor(FrameBufferColors::C255);
    buffer.drawRect(getPosition().getX() + 2, getPosition().getY() + 3, 10, 4);
  }

  buffer.setColor(m_inidicateII ? FrameBufferColors::C179 : FrameBufferColors::C128);
  buffer.fillRect(getPosition().getX() + 2, getPosition().getY() + 9, 10, 4);
  if(m_selectedII)
  {
    buffer.setColor(FrameBufferColors::C255);
    buffer.drawRect(getPosition().getX() + 2, getPosition().getY() + 9, 10, 4);
  }
  return true;
}

bool DualPresetTypeLabel::drawSplit(FrameBuffer &buffer)
{
  auto bgRect = getPosition();
  bgRect.setWidth(15);
  bgRect.setHeight(15);
  buffer.setColor(FrameBufferColors::C43);
  buffer.fillRect(bgRect);

  buffer.setColor(m_inidicateI ? FrameBufferColors::C179 : FrameBufferColors::C128);
  buffer.fillRect(getPosition().getX() + 2, getPosition().getY() + 3, 4, 10);
  if(m_selectedI)
  {
    buffer.setColor(FrameBufferColors::C255);
    buffer.drawRect(getPosition().getX() + 2, getPosition().getY() + 3, 4, 10);
  }

  buffer.setColor(m_inidicateII ? FrameBufferColors::C179 : FrameBufferColors::C128);
  buffer.fillRect(getPosition().getX() + 8, getPosition().getY() + 3, 4, 10);
  if(m_selectedII)
  {
    buffer.setColor(FrameBufferColors::C255);
    buffer.drawRect(getPosition().getX() + 8, getPosition().getY() + 3, 4, 10);
  }
  return true;
}

bool DualPresetTypeLabel::drawSingle(FrameBuffer &buffer)
{
  buffer.setColor(m_inidicateI ? FrameBufferColors::C179 : FrameBufferColors::C128);
  buffer.fillRect(getPosition().getX() + 2, getPosition().getY() + 3, 10, 10);
  if(m_selectedI)
  {
    buffer.setColor(FrameBufferColors::C255);
    buffer.drawRect(getPosition().getX() + 2, getPosition().getY() + 3, 10, 10);
  }

  return true;
}

void DualPresetTypeLabel::update(const Preset *selected)
{
  hasPreset = selected != nullptr;

  if(selected)
  {
    auto currentVGFocus = Application::get().getHWUI()->getCurrentVoiceGroup();
    const auto origin = Application::get().getPresetManager()->getEditBuffer()->getPartOrigin(currentVGFocus);

    auto currentLayout = Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().getLayout().get();
    auto presetManagerLayout = dynamic_cast<PresetManagerLayout *>(currentLayout);

    auto selection = presetManagerLayout->getPresetPartSelection(currentVGFocus);

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
