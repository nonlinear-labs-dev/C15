#include "ParameterInfoText.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/descriptions/ParameterDescriptionDatabase.h>

ParameterInfoText::ParameterInfoText(ControlOwner *parent)
    : super("")
    , m_parent(parent)
{
  setPosition(Rect(2, 0, 0, 0));
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      mem_fun(this, &ParameterInfoText::loadInfoText));
}

void ParameterInfoText::loadInfoText(Parameter *oldParam, Parameter *newParam)
{
  if(newParam)
  {
    auto &db = ParameterDescriptionDatabase::get();
    m_connection.disconnect();
    m_connection = db.load(newParam->getID(), mem_fun(this, &ParameterInfoText::onTextLoaded));
    scrollTop();
  }
}

void ParameterInfoText::onTextLoaded(const Glib::ustring &text)
{
  setText(text, FrameBuffer::Colors::C128);
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

Oleds::tFont ParameterInfoText::getFont()
{
  return Oleds::get().getFont("Emphase_8_TXT_Regular", 8);
}
