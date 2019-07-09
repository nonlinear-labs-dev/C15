#include "ModulationSourceLabel.h"
#include "Application.h"
#include "parameters/ModulateableParameter.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "groups/MacroControlsGroup.h"
#include "proxies/hwui/panel-unit/boled/BOLED.h"

ModulationSourceLabel::ModulationSourceLabel(const Rect &r, Font::Justification justification)
    : super(r)
    , m_justification(justification)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &ModulationSourceLabel::onParameterSelected)));
}

ModulationSourceLabel::~ModulationSourceLabel()
{
}

void ModulationSourceLabel::onParameterSelected(Parameter *parameter)
{
  if(parameter)
  {
    m_paramValueConnection.disconnect();
    m_paramValueConnection
        = parameter->onParameterChanged(sigc::mem_fun(this, &ModulationSourceLabel::onParamValueChanged));
  }
}

void ModulationSourceLabel::onParamValueChanged(const Parameter *param)
{
  if(const auto *modP = dynamic_cast<const ModulateableParameter *>(param))
  {
    uint16_t id = MacroControlsGroup::modSrcToParamID(modP->getModulationSource());

    if(auto mc = Application::get().getPresetManager()->getEditBuffer()->findParameterByID(id))
    {
      setText(mc->getShortName());
      return;
    }
  }

  setText("[-]");
}

bool ModulationSourceLabel::redraw(FrameBuffer &fb)
{
  if(!isHighlight() && getText().text == "[-]")
    return true;

  super::redraw(fb);
  return true;
}

Font::Justification ModulationSourceLabel::getJustification() const
{
  return m_justification;
}
