#include "ModulationSourceEnabledDottedLine.h"
#include "Application.h"
#include "parameters/ModulateableParameter.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include <sigc++/sigc++.h>
#include <proxies/hwui/HWUI.h>

ModulationSourceEnabledDottedLine::ModulationSourceEnabledDottedLine(const Rect &rect)
    : super(rect)
    , m_enabled(false)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &ModulationSourceEnabledDottedLine::onParameterSelected)),
      getHWUI()->getCurrentVoiceGroup());
}

ModulationSourceEnabledDottedLine::~ModulationSourceEnabledDottedLine()
{
}

void ModulationSourceEnabledDottedLine::onParameterSelected(Parameter *parameter)
{
  if(parameter)
  {
    m_paramValueConnection.disconnect();
    m_paramValueConnection
        = parameter->onParameterChanged(sigc::mem_fun(this, &ModulationSourceEnabledDottedLine::onParamValueChanged));
  }
}

void ModulationSourceEnabledDottedLine::onParamValueChanged(const Parameter *param)
{
  if(const ModulateableParameter *modP = dynamic_cast<const ModulateableParameter *>(param))
    setEnabled(modP->getModulationSource() != MacroControls::NONE);
  else
    setEnabled(false);
}

void ModulationSourceEnabledDottedLine::setEnabled(bool e)
{
  if(m_enabled != e)
  {
    m_enabled = e;
    setDirty();
  }
}

bool ModulationSourceEnabledDottedLine::redraw(FrameBuffer &fb)
{
  if(m_enabled)
    super::redraw(fb);

  return true;
}
