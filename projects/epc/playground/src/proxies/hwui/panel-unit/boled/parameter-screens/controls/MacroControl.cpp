#include "MacroControl.h"
#include "proxies/hwui/controls/LabelRegular8.h"
#include "proxies/hwui/controls/Slider.h"
#include "parameters/MacroControlParameter.h"
#include "parameters/ModulateableParameter.h"
#include "groups/MacroControlsGroup.h"
#include "Application.h"
#include <proxies/hwui/HWUI.h>
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include <sigc++/sigc++.h>

MacroControl::MacroControl(const Rect &pos)
    : super(pos)
{
  m_name = addControl(new LabelRegular8("", Rect(0, 0, 48, 13)));
  m_slider = addControl(new Slider(Rect(0, 13, 48, 4)));

  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &MacroControl::onSelectionChanged)), getHWUI()->getCurrentVoiceGroup());
}

MacroControl::~MacroControl()
{
}

void MacroControl::onSelectionChanged(Parameter *parameter, SignalOrigin signalType)
{
  m_connectionTarget.disconnect();
  m_connectionTarget = parameter->onParameterChanged(sigc::mem_fun(this, &MacroControl::onTargetChanged));
}

void MacroControl::onTargetChanged(const Parameter *modulatedParameter)
{
  MacroControlParameter *srcParam = nullptr;

  if(const auto *p = dynamic_cast<const ModulateableParameter *>(modulatedParameter))
  {
    auto src = p->getModulationSource();
    auto srcParamID = MacroControlsGroup::modSrcToParamId(src);
    auto vg = p->getVoiceGroup();

    if(auto pa = Application::get().getPresetManager()->getEditBuffer()->findParameterByID(srcParamID))
      srcParam = dynamic_cast<MacroControlParameter *>(pa);
  }

  if(m_srcParam != srcParam)
  {
    m_srcParam = srcParam;

    m_slider->setParameter(srcParam, SignalOrigin::EXPLICIT);

    if(srcParam)
      m_name->setText(srcParam->getShortName());
    else
      m_name->setText("");
  }
}
