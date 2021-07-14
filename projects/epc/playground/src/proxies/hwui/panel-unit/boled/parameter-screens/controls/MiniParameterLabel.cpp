#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include "MiniParameterLabel.h"
#include <parameters/Parameter.h>
#include <proxies/hwui/FrameBuffer.h>
#include <sigc++/sigc++.h>

MiniParameterLabel::MiniParameterLabel(Parameter *parameter, const Rect &pos)
    : super(parameter->getMiniParameterEditorName(), pos)
    , m_selected(false)
{
  parameter->onParameterChanged(sigc::mem_fun(this, &MiniParameterLabel::onParameterChanged));

  Application::get().getPresetManager()->getEditBuffer()->onPresetLoaded(
      sigc::bind(sigc::mem_fun(this, &MiniParameterLabel::onParameterChanged), parameter));

  Application::get().getPresetManager()->getEditBuffer()->onRecallValuesChanged(
      sigc::bind(sigc::mem_fun(this, &MiniParameterLabel::onParameterChanged), parameter));
}

MiniParameterLabel::~MiniParameterLabel()
{
}

void MiniParameterLabel::onParameterChanged(const Parameter *p)
{
  auto stringPlusLen = prepareDisplayString(p);
  setText(StringAndSuffix { stringPlusLen.first, stringPlusLen.second });
}

void MiniParameterLabel::setSelected(bool selected)
{
  if(m_selected != selected)
  {
    m_selected = selected;
    setDirty();
  }
}

bool MiniParameterLabel::isSelected() const
{
  return m_selected;
}

bool MiniParameterLabel::redraw(FrameBuffer &fb)
{
  auto r = getPosition();

  if(m_selected)
  {
    fb.setColor(FrameBufferColors::C103);
    fb.fillRect(r.getX(), r.getY(), r.getWidth(), r.getHeight());
  }

  super::redraw(fb);
  return true;
}

void MiniParameterLabel::setFontColor(FrameBuffer &fb) const
{
  if(m_selected && isHighlight())
    fb.setColor(FrameBufferColors::C255);
  else
    fb.setColor(FrameBufferColors::C128);
}

const std::pair<Glib::ustring, size_t> MiniParameterLabel::prepareDisplayString(const Parameter *p) const
{
  auto changed = p->isChangedFromLoaded();
  auto text = p->getMiniParameterEditorName() + (changed ? "*" : "");
  size_t suffixLen = changed ? 1 : 0;
  return std::make_pair(text, suffixLen);
}
