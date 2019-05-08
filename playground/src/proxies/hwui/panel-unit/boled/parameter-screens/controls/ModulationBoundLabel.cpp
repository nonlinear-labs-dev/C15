#include <Application.h>
#include <groups/MacroControlsGroup.h>
#include <parameters/MacroControlParameter.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationBoundLabel.h>

ModulationBoundLabel::ModulationBoundLabel(const Rect &r)
    : super(r)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      mem_fun(this, &ModulationBoundLabel::onParameterSelectionChanged));

  Application::get().getHWUI()->onModifiersChanged(mem_fun(this, &ModulationBoundLabel::onButtonModifiersChanged));
}

ModulationBoundLabel::~ModulationBoundLabel()
{
}

void ModulationBoundLabel::onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
{
  m_paramConnection.disconnect();

  if((m_modulatedParam = dynamic_cast<ModulateableParameter *>(newParam)))
  {
    m_paramConnection = m_modulatedParam->onParameterChanged(mem_fun(this, &ModulationBoundLabel::onParameterChanged));
  }
  else
  {
    setText("", 0);
  }
}

void ModulationBoundLabel::onParameterChanged(const Parameter *param)
{
  if(auto m = dynamic_cast<const ModulateableParameter *>(param))
  {
    if(m_mc != m->getModulationSource())
    {
      m_mc = m->getModulationSource();
      m_mcConnection.disconnect();

      auto mcID = MacroControlsGroup::modSrcToParamID(m_mc);
      auto editBuffer = Application::get().getPresetManager()->getEditBuffer();

      if((m_mcParam = dynamic_cast<MacroControlParameter *>(editBuffer->findParameterByID(mcID))))
      {
        m_mcConnection = m_mcParam->onParameterChanged(mem_fun(this, &ModulationBoundLabel::onMCParameterChanged));
      }
      else
      {
        setText("", 0);
      }
    }
  }

  updateText();
}

void ModulationBoundLabel::onMCParameterChanged(const Parameter *mcParam)
{
  updateText();
}

void ModulationBoundLabel::onButtonModifiersChanged(ButtonModifiers mod)
{
  updateText();
}

void ModulationBoundLabel::updateText()
{
  if(m_mcParam && m_modulatedParam)
    updateText(m_mcParam, m_modulatedParam);
}

ModulateableParameter *ModulationBoundLabel::getModulatedParameter()
{
  return m_modulatedParam;
}
