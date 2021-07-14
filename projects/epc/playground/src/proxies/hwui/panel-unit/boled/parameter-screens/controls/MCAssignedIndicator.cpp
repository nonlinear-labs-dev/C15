#include "MCAssignedIndicator.h"
#include <proxies/hwui/FrameBuffer.h>
#include <parameters/MacroControlParameter.h>
#include <parameters/ModulateableParameter.h>
#include <groups/ParameterGroup.h>
#include <proxies/hwui/controls/LabelRegular8.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>

MCAssignedIndicator::MCAssignedIndicator(const Rect& r, const Parameter* p)
    : ControlWithChildren(r)
    , m_parameter { dynamic_cast<const MacroControlParameter*>(p) }
{
  auto pos = r;
  pos.setWidth(40);
  pos.setLeft(r.getLeft());
  pos.setTop(pos.getTop() + 9);
  pos.setHeight(8);

  m_middleRowLabel = addControl(new LabelRegular8(pos));
  m_middleRowLabel->setJustification(Font::Justification::Right);

  Application::get().getHWUI()->onCurrentVoiceGroupChanged(
      sigc::mem_fun(this, &MCAssignedIndicator::onVoiceGroupChanged));
}

bool MCAssignedIndicator::redraw(FrameBuffer& fb)
{
  if(m_parameter)
  {
    drawBackground(fb);
  }

  if(!m_parameter->getTargets().empty())
  {
    drawNonLEDTargets(fb);
  }

  return true;
}

void MCAssignedIndicator::drawArrow(FrameBuffer& fb) const
{
  if(isHighlight())
    fb.setColor(FrameBufferColors::C128);
  else
    fb.setColor(FrameBufferColors::C204);

  Rect r = getPosition();
  Point c = r.getCenter();

  const auto margin = m_middleRowLabel->getWidth();
  const auto middle = r.getCenter().getY();

  fb.setPixel(r.getLeft() + margin + 1, middle - 1);
  fb.setPixel(r.getLeft() + margin + 2, middle - 2);
  fb.setPixel(r.getLeft() + margin + 1, middle + 1);
  fb.setPixel(r.getLeft() + margin + 2, middle + 2);

  for(int i = r.getLeft() + margin; i <= r.getRight(); i += 2)
    fb.setPixel(i, middle);
}

void MCAssignedIndicator::drawSingle(FrameBuffer& fb, const AffectedGroups& mods)
{
  std::string indicator;

  if(mods.mono[0])
    indicator += "\uE040 ";

  if(mods.unison[0])
    indicator += "\uE041 ";

  if(mods.scale)
    indicator += "S ";

  if(mods.master)
    indicator += "M ";

  m_middleRowLabel->setText(StringAndSuffix { indicator, 0 });
  m_middleRowLabel->redraw(fb);

  if(mods.affected(SoundType::Single))
  {
    drawArrow(fb);
  }
}

void MCAssignedIndicator::drawSplit(FrameBuffer& fb, const AffectedGroups& mods)
{
  std::string text;

  int partIdx = mods.currentHWUIVG == VoiceGroup::I ? 0 : 1;

  if(mods.part[partIdx])
    text += "P ";

  if(mods.mono[partIdx])
    text += "\uE040 ";

  if(mods.unison[partIdx])
    text += "\uE041 ";

  if(mods.scale)
    text += "S ";

  if(mods.master)
    text += "M ";

  m_middleRowLabel->setText(StringAndSuffix { text, 0 });
  m_middleRowLabel->redraw(fb);

  if(mods.affected(SoundType::Split))
  {
    drawArrow(fb);
  }
}

void MCAssignedIndicator::drawBackground(FrameBuffer& fb)
{
  fb.setColor(FrameBufferColors::C43);
  fb.fillRect(getPosition());
}

void MCAssignedIndicator::drawLayer(FrameBuffer& fb, const AffectedGroups& mods)
{
  std::string indicaton;
  int idx = mods.currentHWUIVG == VoiceGroup::I ? 0 : 1;

  if(mods.part[idx])
    indicaton += "P ";

  if(mods.mono[0])
    indicaton += "\uE040 ";

  if(mods.unison[0])
    indicaton += "\uE041 ";

  if(mods.scale)
    indicaton += "S ";

  if(mods.master)
    indicaton += "M ";

  m_middleRowLabel->setText(StringAndSuffix { indicaton, 0 });
  m_middleRowLabel->redraw(fb);

  if(mods.affected(SoundType::Layer))
  {
    drawArrow(fb);
  }
}

void MCAssignedIndicator::drawNonLEDTargets(FrameBuffer& fb)
{
  auto targets = m_parameter->getTargets();

  AffectedGroups mods {};

  mods.currentHWUIVG = Application::get().getHWUI()->getCurrentVoiceGroup();

  auto color = isHighlight() ? FrameBufferColors::C128 : FrameBufferColors::C204;

  m_middleRowLabel->setText(StringAndSuffix { "", 0 });
  m_middleRowLabel->setFontColor(color);

  for(auto& t : targets)
  {
    auto groupName = t->getParentGroup()->getID().getName();
    mods.scale |= groupName == "Scale";
    mods.master |= groupName == "Master";

    if(t->getID().getVoiceGroup() != VoiceGroup::Global)
    {
      auto index = t->getID().getVoiceGroup() == VoiceGroup::I ? 0 : 1;
      mods.unison.at(index) |= groupName == "Unison";
      mods.mono.at(index) |= groupName == "Mono";
      mods.part.at(index) |= groupName == "Part";
      mods.part.at(index) |= groupName == "Split";
    }
  }

  switch(Application::get().getPresetManager()->getEditBuffer()->getType())
  {
    case SoundType::Split:
      drawSplit(fb, mods);
      break;
    case SoundType::Layer:
      drawLayer(fb, mods);
      break;
    default:
    case SoundType::Single:
      drawSingle(fb, mods);
      break;
  }
}

void MCAssignedIndicator::onVoiceGroupChanged(VoiceGroup vg)
{
  setDirty();
}
