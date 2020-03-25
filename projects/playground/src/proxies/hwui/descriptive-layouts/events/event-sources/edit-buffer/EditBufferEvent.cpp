#include "EditBufferEvent.h"
#include <presets/EditBuffer.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/HWUI.h>

DescriptiveLayouts::EditBufferEventBase::EditBufferEventBase()
{
  auto eb = getEditBuffer();
  m_changedConnection = eb->onChange(sigc::mem_fun(this, &EditBufferEventBase::onEditBufferChanged));
  m_presetLoadedConnection = eb->onPresetLoaded(sigc::mem_fun(this, &EditBufferEventBase::onPresetLoaded));
  m_onRecallChanged = eb->onRecallValuesChanged(sigc::mem_fun(this, &EditBufferEventBase::onRecallChanged));
  m_onHardwareUIVoiceGroupSelectionChanged = Application::get().getHWUI()->onCurrentVoiceGroupChanged(
      sigc::mem_fun(this, &EditBufferEventBase::onHWUIVoiceGroupSelectionChanged));
}

DescriptiveLayouts::EditBufferEventBase::~EditBufferEventBase()
{
  m_changedConnection.disconnect();
  m_presetLoadedConnection.disconnect();
  m_onRecallChanged.disconnect();
}

void DescriptiveLayouts::EditBufferEventBase::onEditBufferChanged()
{
  onChange(getEditBuffer());
}

void DescriptiveLayouts::EditBufferEventBase::onPresetLoaded()
{
  onChange(getEditBuffer());
}

void DescriptiveLayouts::EditBufferEventBase::onRecallChanged()
{
  onChange(getEditBuffer());
}

void DescriptiveLayouts::EditBufferEventBase::onHWUIVoiceGroupSelectionChanged(VoiceGroup v)
{
  onChange(getEditBuffer());
}

EditBuffer *DescriptiveLayouts::EditBufferEventBase::getEditBuffer()
{
  return Application::get().getPresetManager()->getEditBuffer();
}

DescriptiveLayouts::VoiceGroupSelectEventBase::VoiceGroupSelectEventBase()
{
  m_vgSelection = Application::get().getHWUI()->onCurrentVoiceGroupChanged(
      sigc::mem_fun(this, &VoiceGroupSelectEventBase::onChange));
}
