#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include "VoiceGroupIndicator.h"
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/controls/SwitchVoiceGroupButton.h>
#include <groups/MacroControlsGroup.h>
#include <parameter_declarations.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/PresetManagerLayout.h>

VoiceGroupIndicator::VoiceGroupIndicator(const Rect& r, bool allowLoadToPart)
    : Control(r)
    , m_allowLoadToPart(allowLoadToPart)
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();

  eb->onSoundTypeChanged(sigc::mem_fun(this, &VoiceGroupIndicator::onSoundTypeChanged));

  eb->onSelectionChanged(sigc::mem_fun(this, &VoiceGroupIndicator::onParameterSelectionChanged),
                         getHWUI()->getCurrentVoiceGroup());

  Application::get().getHWUI()->onCurrentVoiceGroupChanged(
      sigc::mem_fun(this, &VoiceGroupIndicator::onVoiceGroupSelectionChanged));

  Application::get().getHWUI()->onLoadToPartModeChanged(sigc::mem_fun(this, &VoiceGroupIndicator::onLoadModeChanged));
}

VoiceGroupIndicator::~VoiceGroupIndicator()
{
  m_parameterChanged.disconnect();
}

bool VoiceGroupIndicator::redraw(FrameBuffer& fb)
{
  if(shouldDraw())
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
  fb.fillRect(Rect(absPos.getLeft(), absPos.getTop(), 12, 5));
  fb.setColor(m_selectedVoiceGroup == VoiceGroup::II ? FrameBufferColors::C255 : FrameBufferColors::C128);
  fb.fillRect(Rect(absPos.getLeft(), absPos.getTop() + 7, 12, 5));

  if(m_inLoadToPart)
  {
    const auto startX = absPos.getLeft() + 13;
    auto startY = absPos.getTop() + (m_selectedVoiceGroup == VoiceGroup::I ? 2 : 9);
    fb.setColor(FrameBufferColors::C255);
    fb.drawHorizontalLine(startX + 2, startY - 2, 1);
    fb.drawHorizontalLine(startX + 1, startY - 1, 2);
    fb.drawHorizontalLine(startX, startY, 3);
    fb.drawHorizontalLine(startX + 1, startY + 1, 2);
    fb.drawHorizontalLine(startX + 2, startY + 2, 1);
  }

  if(isLayerPartMuted(VoiceGroup::I))
  {
    fb.setColor(FrameBufferColors::C43);

    auto centerX = absPos.getLeft() + 5;
    auto centerY = absPos.getTop() + 2;
    fb.setPixel(centerX - 2, centerY - 2);
    fb.setPixel(centerX - 2, centerY + 2);
    fb.setPixel(centerX - 1, centerY - 1);
    fb.setPixel(centerX - 1, centerY + 1);
    fb.setPixel(centerX, centerY);
    fb.setPixel(centerX + 1, centerY);
    fb.setPixel(centerX + 3, centerY - 2);
    fb.setPixel(centerX + 3, centerY + 2);
    fb.setPixel(centerX + 2, centerY - 1);
    fb.setPixel(centerX + 2, centerY + 1);
  }

  if(isLayerPartMuted(VoiceGroup::II))
  {
    fb.setColor(FrameBufferColors::C43);

    auto centerX = absPos.getLeft() + 5;
    auto centerY = absPos.getTop() + 9;
    fb.setPixel(centerX - 2, centerY - 2);
    fb.setPixel(centerX - 2, centerY + 2);
    fb.setPixel(centerX - 1, centerY - 1);
    fb.setPixel(centerX - 1, centerY + 1);
    fb.setPixel(centerX, centerY);
    fb.setPixel(centerX + 1, centerY);
    fb.setPixel(centerX + 3, centerY - 2);
    fb.setPixel(centerX + 3, centerY + 2);
    fb.setPixel(centerX + 2, centerY - 1);
    fb.setPixel(centerX + 2, centerY + 1);
  }

  return true;
}

bool VoiceGroupIndicator::drawSplit(FrameBuffer& fb)
{
  auto absPos = getPosition();
  fb.setColor(m_selectedVoiceGroup == VoiceGroup::I ? FrameBufferColors::C255 : FrameBufferColors::C128);
  fb.fillRect(Rect(absPos.getLeft(), absPos.getTop(), 5, 12));
  fb.setColor(m_selectedVoiceGroup == VoiceGroup::II ? FrameBufferColors::C255 : FrameBufferColors::C128);
  fb.fillRect(Rect(absPos.getLeft() + 7, absPos.getTop(), 5, 12));

  if(m_inLoadToPart)
  {
    const auto startY = absPos.getTop() + 13;
    auto startX = absPos.getLeft() + (m_selectedVoiceGroup == VoiceGroup::I ? 2 : 9);
    fb.setColor(FrameBufferColors::C255);
    fb.drawVerticalLine(startX - 2, startY + 2, 1);
    fb.drawVerticalLine(startX - 1, startY + 1, 2);
    fb.drawVerticalLine(startX, startY, 3);
    fb.drawVerticalLine(startX + 1, startY + 1, 2);
    fb.drawVerticalLine(startX + 2, startY + 2, 1);
  }

  return true;
}

bool VoiceGroupIndicator::isLayerPartMuted(VoiceGroup vg) const
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  if(auto mute = eb->findParameterByID({ C15::PID::Voice_Grp_Mute, vg }))
  {
    return mute->getControlPositionValue() > 0 && eb->getType() == SoundType::Layer;
  }
  return false;
}

void VoiceGroupIndicator::onSoundTypeChanged(SoundType type)
{
  m_currentSoundType = type;
  setDirty();
}

void VoiceGroupIndicator::onParameterChanged(const Parameter* parameter)
{
  const auto paramNum = parameter->getID().getNumber();

  if(paramNum == C15::PID::Split_Split_Point || MacroControlsGroup::isMacroControl(paramNum))
    m_selectedVoiceGroup = Application::get().getHWUI()->getCurrentVoiceGroup();

  setDirty();
}

void VoiceGroupIndicator::onParameterSelectionChanged(const Parameter* old, const Parameter* newParam,
                                                      SignalOrigin signalType)
{
  m_parameterChanged.disconnect();
  if(newParam)
  {
    m_param = newParam;
    
    if(!ParameterId::isGlobal(newParam->getID().getNumber()))
    {
      m_selectedVoiceGroup = newParam->getID().getVoiceGroup();
      setDirty();
    }

    m_parameterChanged = newParam->onParameterChanged(sigc::mem_fun(this, &VoiceGroupIndicator::onParameterChanged));
  }
}

bool VoiceGroupIndicator::shouldDraw()
{
  if(auto layout = Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().getBaseLayout())
  {
    if(dynamic_cast<PresetManagerLayout*>(layout.get()))
    {
      return m_currentSoundType != SoundType::Single;
    }
  }

  if(m_currentSoundType == SoundType::Single)
    return false;

  if(m_param)
  {
    auto id = m_param->getID();
    auto num = id.getNumber();

    auto ret = SwitchVoiceGroupButton::allowToggling(m_param, Application::get().getPresetManager()->getEditBuffer());
    ret |= MacroControlsGroup::isMacroControl(num);
    ret |= num == C15::PID::Split_Split_Point;

    return ret;
  }
  return false;
}

void VoiceGroupIndicator::onVoiceGroupSelectionChanged(VoiceGroup vg)
{
  m_selectedVoiceGroup = vg;
  setDirty();
}

void VoiceGroupIndicator::onLoadModeChanged(bool loadModeActive)
{
  m_inLoadToPart = m_allowLoadToPart && loadModeActive;
  setDirty();
}
