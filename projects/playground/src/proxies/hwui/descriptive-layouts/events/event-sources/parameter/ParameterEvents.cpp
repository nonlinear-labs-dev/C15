#include "ParameterEvents.h"
#include <groups/ParameterGroup.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>

void DescriptiveLayouts::ParameterGroupNameEventSource::onSelectedParameterChanged(const Parameter *p)
{
  setValue({ p ? p->getParentGroup()->getShortName() : "", 0 });
}

void DescriptiveLayouts::ParameterIsBipolarEventSource::onSelectedParameterChanged(const Parameter *p)
{
  setValue(p ? p->isBiPolar() : false);
}

void DescriptiveLayouts::CurrentParameterControlPosition::onSelectedParameterChanged(const Parameter *p)
{
  if(p)
  {
    setValue(p->getControlPositionValue());
  }
}

void DescriptiveLayouts::ParameterNameEventSource::onSelectedParameterChanged(const Parameter *p)
{
  if(p)
    setValue({ p->getLongName(), 0 });
}

void DescriptiveLayouts::ParameterNameWithStateSuffixEventSource::onSelectedParameterChanged(const Parameter *parameter)
{
  if(parameter)
  {
    auto changed = parameter->isChangedFromLoaded();
    auto displayStr = parameter->getLongName().append(changed ? "*" : "");
    setValue(DisplayString { displayStr, changed ? 1 : 0 });
  }
  else
  {
    setValue(DisplayString { "", 0 });
  }
}

DescriptiveLayouts::ParameterDisplayStringEventSource::ParameterDisplayStringEventSource()
{
  m_modifierConnection = Application::get().getHWUI()->onModifiersChanged(
      sigc::mem_fun(this, &ParameterDisplayStringEventSource::onModifierChanged));
}

DescriptiveLayouts::ParameterDisplayStringEventSource::~ParameterDisplayStringEventSource()
{
  m_modifierConnection.disconnect();
}

void DescriptiveLayouts::ParameterDisplayStringEventSource::onSelectedParameterChanged(const Parameter *p)
{
  auto str = p ? p->getDisplayString() : Glib::ustring {};

  if(Application::get().getHWUI()->isModifierSet(ButtonModifier::FINE))
  {
    setValue({ str + " F", 2 });
  }
  else
  {
    setValue({ str, 0 });
  }
}

void DescriptiveLayouts::ParameterDisplayStringEventSource::onModifierChanged(ButtonModifiers mods)
{
  onSelectedParameterChanged(Application::get().getPresetManager()->getEditBuffer()->getSelected(
      Application::get().getHWUI()->getCurrentVoiceGroup()));
}

void DescriptiveLayouts::ParameterValueChanged::onSelectedParameterChanged(const Parameter *p)
{
  setValue(p && p->isChangedFromLoaded());
}
