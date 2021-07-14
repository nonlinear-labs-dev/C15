#include <proxies/hwui/Oleds.h>
#include <tools/StringTools.h>
#include <nltools/Types.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/FrameBuffer.h>
#include <parameter_declarations.h>
#include <proxies/hwui/controls/PNGControl.h>
#include "SplitParameterValue.h"
#include "proxies/hwui/controls/Label.h"
#include "parameters/SplitPointParameter.h"
#include "device-settings/Settings.h"
#include "device-settings/SplitPointSyncParameters.h"

SplitParameterValue::SplitParameterValue(const Rect& pos)
    : Label(pos)
{
  setFontColor(FrameBufferColors::C179);

  init();
}

SplitParameterValue::SplitParameterValue(const StringAndSuffix& text, const Rect& pos)
    : Label(text, pos)
{
  setFontColor(FrameBufferColors::C179);

  init();
}

void SplitParameterValue::init()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto split1 = eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I });
  auto split2 = eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::II });

  split1->onParameterChanged(sigc::mem_fun(this, &SplitParameterValue::onSplitIChanged));
  split2->onParameterChanged(sigc::mem_fun(this, &SplitParameterValue::onSplitIIChanged));
}

void SplitParameterValue::onSplitIChanged(const Parameter* splitI)
{
  m_splitICP = splitI->getControlPositionValue();
  setDirty();
}

void SplitParameterValue::onSplitIIChanged(const Parameter* splitII)
{
  m_splitIICP = splitII->getControlPositionValue();
  setDirty();
}

void SplitParameterValue::drawParts(FrameBuffer& fb, const std::vector<Glib::ustring>& parts)
{
  auto w = 0;
  for(auto& p : parts)
    w += getFont()->getStringWidth(p);

  int x = getPosition().getCenter().getX() - (w / 2);
  int y = getPosition().getCenter().getY() + 4;

  for(auto i = 0; i < parts.size(); i++)
  {
    auto part = parts[i];
    fb.setColor(getColorForSplit(i));
    getFont()->draw(part, x, y);
    x += getFont()->getStringWidth(part);
  }
}

inline VoiceGroup invert(VoiceGroup vg)
{
  if(vg == VoiceGroup::I)
  {
    return VoiceGroup::II;
  }
  else
  {
    return VoiceGroup::I;
  }
}

FrameBufferColors SplitParameterValue::getColorForSplit(int i)
{
  VoiceGroup selectedVg = Application::get().getHWUI()->getCurrentVoiceGroup();
  const FrameBufferColors highlighted = FrameBufferColors::C255;
  const FrameBufferColors unhighlighted = FrameBufferColors::C128;

  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto sI = eb->findAndCastParameterByID<SplitPointParameter>({ C15::PID::Split_Split_Point, VoiceGroup::I });

  auto biggerPart = sI->hasOverlap() ? VoiceGroup::I : VoiceGroup::II;

  switch(i)
  {
    case 0:
      return selectedVg == invert(biggerPart) ? highlighted : unhighlighted;
    case 2:
      return selectedVg == biggerPart ? highlighted : unhighlighted;
    default:
    case 1:
      return unhighlighted;
  }
}

bool SplitParameterValue::redraw(FrameBuffer& fb)
{
  auto text = getText();
  auto splits = text.getSplits();
  auto parts = StringTools::splitStringOnAnyDelimiter(splits.first, '\t');

  if(parts.size() > 1)
  {
    drawParts(fb, parts);
  }
  else
  {
    Label::redraw(fb);
  }
  return true;
}

StringAndSuffix SplitParameterValue::getText() const
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto sI = dynamic_cast<SplitPointParameter*>(eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I }));
  auto sII = eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::II });
  auto delim = sI->hasOverlap() ? "-" : "|";

  auto biggerPart = sI->getControlPositionValue() > sII->getControlPositionValue() ? VoiceGroup::I : VoiceGroup::II;

  if(biggerPart == VoiceGroup::I)
  {
    return { sII->getDisplayString() + "\t" + delim + "\t" + sI->getDisplayString(), "" };
  }
  else
  {
    return { sI->getDisplayString() + "\t" + delim + "\t" + sII->getDisplayString(), "" };
  }
}
