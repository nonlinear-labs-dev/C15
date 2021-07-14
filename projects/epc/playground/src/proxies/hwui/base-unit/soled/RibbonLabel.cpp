#include "RibbonLabel.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/PhysicalControlParameter.h"
#include "proxies/hwui/HWUI.h"
#include <groups/ParameterGroup.h>

RibbonLabel::RibbonLabel(const ParameterId &paramID, const Rect &rect)
    : super(rect)
    , m_parameterID(paramID)
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  eb->getParameterGroupByID({ "CS", VoiceGroup::Global })->onGroupChanged(mem_fun(this, &RibbonLabel::setDirty));
  eb->getParameterGroupByID({ "MCs", VoiceGroup::Global })->onGroupChanged(mem_fun(this, &RibbonLabel::setDirty));
  eb->getParameterGroupByID({ "MCM", VoiceGroup::Global })->onGroupChanged(mem_fun(this, &RibbonLabel::setDirty));
}

RibbonLabel::~RibbonLabel() = default;

StringAndSuffix RibbonLabel::getText() const
{
  auto param = dynamic_cast<PhysicalControlParameter *>(
      Application::get().getPresetManager()->getEditBuffer()->findParameterByID(m_parameterID));
  return StringAndSuffix { crop(param->generateName()) };
}

Glib::ustring RibbonLabel::crop(const Glib::ustring &text) const
{
  int min = 0;
  int max = text.length();

  auto w = getFont()->getStringWidth(text);

  if(w <= getPosition().getWidth())
    return text;

  return binarySearchLength(text, min, max);
}

Glib::ustring RibbonLabel::binarySearchLength(const Glib::ustring &text, int min, int max) const
{
  if(max == 0)
    return "";

  if((min + 1) == max)
    return text.substr(0, min) + "..";

  int halfIdx = min + (max - min) / 2;
  Glib::ustring half = text.substr(0, halfIdx);
  Glib::ustring toMeasure = half + "..";

  auto w = getFont()->getStringWidth(toMeasure);

  if(w <= getPosition().getWidth())
    min = halfIdx;
  else
    max = halfIdx;

  return binarySearchLength(text, min, max);
}
