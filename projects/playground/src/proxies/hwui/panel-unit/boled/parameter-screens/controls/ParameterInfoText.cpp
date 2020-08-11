#include "ParameterInfoText.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/Parameter.h>
#include <proxies/hwui/Oleds.h>
#include <parameters/names/ParameterDB.h>
#include <parameters/MacroControlParameter.h>
#include <proxies/hwui/HWUI.h>

ParameterInfoText::ParameterInfoText(ControlOwner *parent)
    : super("")
    , m_parent(parent)
{
  setPosition(Rect(2, 0, 0, 0));
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      mem_fun(this, &ParameterInfoText::onParameterSelected), getHWUI()->getCurrentVoiceGroup());
}

void ParameterInfoText::onParameterSelected(Parameter *oldParam, Parameter *newParam)
{
  m_currentParameterConnection.disconnect();

  if(auto modParam = dynamic_cast<MacroControlParameter *>(newParam))
  {
    m_currentParameterConnection
        = modParam->onParameterChanged(sigc::mem_fun(this, &ParameterInfoText::onParameterChanged), true);
  }
  else
  {
    onParameterChanged(newParam);
  }
}

void ParameterInfoText::onParameterChanged(const Parameter *newParameter)
{
  auto &db = ParameterDB::get();
  onTextLoaded(db.getDescription(newParameter->getID()));
  scrollTop();
}

void ParameterInfoText::onTextLoaded(const Glib::ustring &text)
{
  setText(text, FrameBufferColors::C128);
}

void ParameterInfoText::setPosition(const Rect &rect)
{
  super::setPosition(rect);
}

const Rect &ParameterInfoText::getPosition() const
{
  return super::getPosition();
}

void ParameterInfoText::setDirty()
{
  super::setDirty();
  notifyDirty(true);
}

std::shared_ptr<Font> ParameterInfoText::getFont()
{
  return Oleds::get().getFont("Emphase-8-TXT-Regular", 8);
}
