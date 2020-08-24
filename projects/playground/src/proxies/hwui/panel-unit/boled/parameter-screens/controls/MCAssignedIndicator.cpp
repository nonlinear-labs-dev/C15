#include "MCAssignedIndicator.h"
#include <proxies/hwui/FrameBuffer.h>
#include <parameters/MacroControlParameter.h>
#include <parameters/ModulateableParameter.h>
#include <groups/ParameterGroup.h>
#include <proxies/hwui/controls/LabelRegular8.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>

MCAssignedIndicator::MCAssignedIndicator(const Rect& r, const Parameter* p)
    : ControlWithChildren(r)
    , m_parameter { dynamic_cast<const MacroControlParameter*>(p) }
{
  auto pos = r;
  pos.setWidth(25);
  pos.setLeft(r.getLeft());
  pos.setHeight(8);

  auto offset = 0;

  m_topRowLabel = addControl(new LabelRegular8(pos));
  m_middleRowLabel = addControl(new LabelRegular8(pos));
  m_bottomRowLabel = addControl(new LabelRegular8(pos));

  for(auto l : { m_topRowLabel, m_middleRowLabel, m_bottomRowLabel })
  {
    pos.setTop(r.getTop() + offset);
    l->setPosition(pos);
    l->setJustification(Font::Justification::Right);
    offset += 8;
  }
}

bool MCAssignedIndicator::redraw(FrameBuffer& fb)
{
  if(m_parameter && m_parameter->getTargets().empty())
  {
    drawBackground(fb);
    return false;
  }

  drawNonLEDTargets(fb);
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

  if(!indicator.empty())
    indicator.pop_back();

  m_middleRowLabel->setText({ indicator, 0 });
  m_middleRowLabel->redraw(fb);

  if(mods.affected(SoundType::Single))
  {
    drawArrow(fb);
  }
}

void MCAssignedIndicator::drawSplit(FrameBuffer& fb, const AffectedGroups& mods)
{
  std::string globalIndicator;
  std::string partIIndicator;
  std::string partIIIndicator;

  if(mods.mono[0])
    partIIndicator += "\uE040 ";

  if(mods.mono[1])
    partIIIndicator += "\uE040 ";

  if(mods.unison[0])
    partIIndicator += "\uE041 ";

  if(mods.unison[1])
    partIIIndicator += "\uE041 ";

  if(mods.scale)
    globalIndicator += "S ";

  if(mods.master)
    globalIndicator += "M ";

  if(mods.part[0])
    partIIndicator += "P ";

  if(mods.part[1])
    partIIIndicator += "P ";

  if(!partIIndicator.empty())
    partIIndicator.pop_back();

  if(!partIIIndicator.empty())
    partIIIndicator.pop_back();

  if(!globalIndicator.empty())
    globalIndicator.pop_back();

  m_topRowLabel->setText({ partIIndicator, 0 });
  m_topRowLabel->redraw(fb);

  m_middleRowLabel->setText({ globalIndicator, 0 });
  m_middleRowLabel->redraw(fb);

  m_bottomRowLabel->setText({ partIIIndicator, 0 });
  m_bottomRowLabel->redraw(fb);

  if(mods.affected(SoundType::Split))
  {
    drawArrow(fb);
  }
}

void MCAssignedIndicator::drawLayer(FrameBuffer& fb, const AffectedGroups& mods)
{
  std::string indicaton;

  if(mods.mono[0])
    indicaton += "\uE040 ";

  if(mods.unison[0])
    indicaton += "\uE041 ";

  if(mods.scale)
    indicaton += "S ";

  if(mods.master)
    indicaton += "M ";

  if(mods.part[0])
  {
    m_topRowLabel->setText({ "P", 0 });
    m_topRowLabel->redraw(fb);
  }

  if(mods.part[1])
  {
    m_bottomRowLabel->setText({ "P", 0 });
    m_bottomRowLabel->redraw(fb);
  }

  if(!indicaton.empty())
    indicaton.pop_back();

  m_middleRowLabel->setText({ indicaton, 0 });
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

  auto color = isHighlight() ? FrameBufferColors::C128 : FrameBufferColors::C204;

  for(auto l : { m_topRowLabel, m_middleRowLabel, m_bottomRowLabel })
  {
    l->setText({ "", 0 });
    l->setFontColor(color);
  }

  for(auto& t : targets)
  {
    auto groupName = t->getParentGroup()->getID().getName();
    mods.scale |= groupName == "Scale";
    mods.master |= groupName == "Master";

    if(t->getID().getVoiceGroup() != VoiceGroup::Global)
    {
      auto index = static_cast<int>(t->getID().getVoiceGroup());
      mods.unison.at(index) |= groupName == "Unison";
      mods.mono.at(index) |= groupName == "Mono";
      mods.part.at(index) |= groupName == "Part";
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
    case SoundType::Single:
      drawSingle(fb, mods);
      break;
  }
}