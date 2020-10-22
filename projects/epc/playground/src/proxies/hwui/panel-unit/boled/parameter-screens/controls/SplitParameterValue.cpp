#include <proxies/hwui/Oleds.h>
#include <tools/StringTools.h>
#include <nltools/Types.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/FrameBuffer.h>
#include <parameter_declarations.h>
#include "SplitParameterValue.h"
#include "proxies/hwui/controls/Label.h"

SplitParameterValue::SplitParameterValue(const Rect& pos)
    : Label(pos)
{
  setFontColor(FrameBufferColors::C179);
}

SplitParameterValue::SplitParameterValue(const Label::StringAndSuffix& text, const Rect& pos)
    : Label(text, pos)
{
  setFontColor(FrameBufferColors::C179);
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

FrameBufferColors SplitParameterValue::getColorForSplit(int i)
{
  VoiceGroup selectedVg = Application::get().getHWUI()->getCurrentVoiceGroup();
  const FrameBufferColors highlighted = FrameBufferColors::C255;

  switch(i)
  {
    case 0:
      return selectedVg == VoiceGroup::I ? highlighted : getFontColor();
    case 2:
      return selectedVg == VoiceGroup::II ? highlighted : getFontColor();
    default:
    case 1:
      return getFontColor();
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

Label::StringAndSuffix SplitParameterValue::getText() const
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto sI = eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::I });
  auto sII = eb->findParameterByID({ C15::PID::Split_Split_Point, VoiceGroup::II });
  return { sI->getDisplayString() + "\t-\t" + sII->getDisplayString(), "" };
}
