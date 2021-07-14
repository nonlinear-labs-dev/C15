#include <tools/TextSplitter.h>
#include "ParameterNameLabel.h"
#include "Application.h"
#include <proxies/hwui/HWUI.h>
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/Parameter.h"
#include "proxies/hwui/panel-unit/boled/BOLED.h"
#include <proxies/hwui/Oleds.h>
#include <proxies/hwui/FrameBuffer.h>
#include <sigc++/sigc++.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterLayout.h>

ParameterNameLabel::ParameterNameLabel(const Rect &pos)
    : Label(pos)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &ParameterNameLabel::onParameterSelected)), getHWUI()->getCurrentVoiceGroup());

  Application::get().getPresetManager()->getEditBuffer()->onPresetLoaded(
      sigc::mem_fun(this, &ParameterNameLabel::onPresetLoaded));

  Application::get().getPresetManager()->getEditBuffer()->onRecallValuesChanged(
      sigc::mem_fun(this, &ParameterNameLabel::onPresetLoaded));
}

ParameterNameLabel::~ParameterNameLabel()
{
}

void ParameterNameLabel::onParameterSelected(Parameter *param)
{
  if(param)
  {
    m_connection.disconnect();
    m_connection = param->onParameterChanged(mem_fun(this, &ParameterNameLabel::onParameterChanged));
  }
}

Glib::ustring removeLastChar(const Glib::ustring &s)
{
  if(!s.empty())
    return s.substr(0, s.size() - 1);
  return s;
}

Glib::ustring addStar(const Glib::ustring &s)
{
  return s + "*";
}

void ParameterNameLabel::handleMCParameterName(const Parameter *pParameter)
{
  const auto changed = pParameter->isChangedFromLoaded();
  auto name = pParameter->getLongName();
  name = truncateMCName(changed, name);
  setText({ name, changed ? 1u : 0u });
}

Glib::ustring ParameterNameLabel::truncateMCName(const bool changed, const Glib::ustring &name) const
{
  Glib::ustring ret = name;

  if(getFont()->getStringWidth(ret + (changed ? "*" : "")) > getWidth())
  {
    ret = removeLastChar(ret);

    while(getFont()->getStringWidth(Glib::ustring(ret + ".." + (changed ? "*" : ""))) > getWidth())
    {
      ret = removeLastChar(ret);
      if(ret.empty())
        break;
    }

    ret += "..";
  }

  if(changed)
    ret = addStar(ret);

  return ret;
}

void ParameterNameLabel::handleParameterName(const Parameter *pParameter)
{
  const auto changed = pParameter->isChangedFromLoaded();
  setText({ pParameter->getLongName() + (changed ? "*" : ""), changed ? 1u : 0u });
}

void ParameterNameLabel::onParameterChanged(const Parameter *param)
{
  auto id = param->getID().getNumber();

  if(id == 243 || id == 244 || id == 245 || id == 246 || id == 369 || id == 371)
  {
    handleMCParameterName(param);
  }
  else
  {
    handleParameterName(param);
  }
}

void ParameterNameLabel::setFontColor(FrameBuffer &fb) const
{
  if(isHighlight())
    fb.setColor(FrameBufferColors::C255);
  else
    fb.setColor(FrameBufferColors::C128);
}

std::shared_ptr<Font> ParameterNameLabel::getFont() const
{
  return Oleds::get().getFont("Emphase-9-Bold", getFontHeight());
}

int ParameterNameLabel::getFontHeight() const
{
  return 9;
}

void ParameterNameLabel::setSuffixFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBufferColors::C103);
}

void ParameterNameLabel::onPresetLoaded()
{
  const auto param
      = Application::get().getPresetManager()->getEditBuffer()->getSelected(getHWUI()->getCurrentVoiceGroup());
  onParameterChanged(param);
}
