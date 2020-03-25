#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include "VoiceGroupIndicator.h"
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/controls/SwitchVoiceGroupButton.h>

VoiceGroupIndicator::VoiceGroupIndicator(const Rect& r)
    : Control(r)
{
  m_soundTypeChanged = Application::get().getPresetManager()->getEditBuffer()->onSoundTypeChanged(
      sigc::mem_fun(this, &VoiceGroupIndicator::onSoundTypeChanged));

  auto parameter = Application::get().getPresetManager()->getEditBuffer()->getSelected();
  m_parameterSelectionChanged
      = parameter->onParameterChanged(sigc::mem_fun(this, &VoiceGroupIndicator::onParameterChanged));
}

VoiceGroupIndicator::~VoiceGroupIndicator()
{
  m_soundTypeChanged.disconnect();
  m_parameterSelectionChanged.disconnect();
}

bool VoiceGroupIndicator::redraw(FrameBuffer& fb)
{
  if(m_shouldDraw)
  {
    if(m_currentSoundType == SoundType::Split)
      return drawSplit(fb);
    else if(m_currentSoundType == SoundType::Layer)
      return drawLayer(fb);
  }

  return true;
}

bool VoiceGroupIndicator::drawLayer(FrameBuffer& fb)
{
  auto absPos = getPosition();
  fb.setColor(m_selectedVoiceGroup == VoiceGroup::I ? FrameBufferColors::C255 : FrameBufferColors::C128);
  fb.fillRect(Rect(absPos.getLeft(), absPos.getTop(), 11, 5));
  fb.setColor(m_selectedVoiceGroup == VoiceGroup::II ? FrameBufferColors::C255 : FrameBufferColors::C128);
  fb.fillRect(Rect(absPos.getLeft(), absPos.getTop() + 6, 11, 5));
  return true;
}

bool VoiceGroupIndicator::drawSplit(FrameBuffer& fb)
{
  auto absPos = getPosition();
  fb.setColor(m_selectedVoiceGroup == VoiceGroup::I ? FrameBufferColors::C255 : FrameBufferColors::C128);
  fb.fillRect(Rect(absPos.getLeft(), absPos.getTop(), 5, 11));
  fb.setColor(m_selectedVoiceGroup == VoiceGroup::II ? FrameBufferColors::C255 : FrameBufferColors::C128);
  fb.fillRect(Rect(absPos.getLeft() + 6, absPos.getTop(), 5, 11));
  return true;
}

void VoiceGroupIndicator::onSoundTypeChanged()
{
  m_currentSoundType = Application::get().getPresetManager()->getEditBuffer()->getType();
  setDirty();
}

void VoiceGroupIndicator::onParameterChanged(const Parameter* parameter)
{
  m_shouldDraw
      = SwitchVoiceGroupButton::allowToggling(parameter, Application::get().getPresetManager()->getEditBuffer());
  m_shouldDraw |= parameter->getParentGroup()->getID().getName() == "MCs";
  m_shouldDraw |= parameter->getParentGroup()->getID().getName() == "Split";
  m_selectedVoiceGroup = parameter->getID().getVoiceGroup();

  onSoundTypeChanged();
  setDirty();
}