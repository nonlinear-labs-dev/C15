#include "MacroControl.h"
#include "proxies/hwui/controls/LabelRegular8.h"
#include "proxies/hwui/controls/Slider.h"
#include "parameters/MacroControlParameter.h"
#include "parameters/ModulateableParameter.h"
#include "groups/MacroControlsGroup.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"

MacroControl::MacroControl(const Rect &pos)
    : super(pos)
{
  m_name = addControl(new LabelRegular8("", Rect(0, 0, 48, 13)));
  m_slider = addControl(new Slider(Rect(0, 13, 48, 4)));

  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &MacroControl::onSelectionChanged)));
}

MacroControl::~MacroControl()
{
}

void MacroControl::onSelectionChanged(Parameter *parameter)
{
  m_connectionTarget.disconnect();
  m_connectionTarget = parameter->onParameterChanged(sigc::mem_fun(this, &MacroControl::onTargetChanged));
}

void MacroControl::onTargetChanged(const Parameter *modulatedParameter)
{
  MacroControlParameter *srcParam = nullptr;

  if(const ModulateableParameter *p = dynamic_cast<const ModulateableParameter *>(modulatedParameter))
  {
    auto src = p->getModulationSource();
    uint16_t srcParamID = MacroControlsGroup::modSrcToParamID(src);

    if(auto pa = Application::get().getPresetManager()->getEditBuffer()->findParameterByID(srcParamID))
      srcParam = dynamic_cast<MacroControlParameter *>(pa);
  }

  if(m_srcParam != srcParam)
  {
    m_srcParam = srcParam;

    m_slider->setParameter(srcParam);

    if(srcParam)
      m_name->setText(srcParam->getShortName());
    else
      m_name->setText("");
  }
}
