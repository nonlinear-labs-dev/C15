#include "SwitchVoiceGroupButton.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <parameters/mono-mode-parameters/UnmodulateableMonoParameter.h>
#include <parameters/mono-mode-parameters/ModulateableMonoParameter.h>
#include <parameters/mono-mode-parameters/MonoGlideTimeParameter.h>
#include <parameters/SplitPointParameter.h>
#include <groups/MonoGroup.h>
#include <proxies/hwui/HWUI.h>
#include <groups/UnisonGroup.h>
#include <nltools/Types.h>
#include <libundo/undo/Scope.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterLayout.h>
#include <parameter_declarations.h>

SwitchVoiceGroupButton::SwitchVoiceGroupButton(Buttons pos)
    : Button("", pos)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::mem_fun(this, &SwitchVoiceGroupButton::onParameterSelectionChanged), getHWUI()->getCurrentVoiceGroup());

  Application::get().getHWUI()->onCurrentVoiceGroupChanged(
      sigc::mem_fun(this, &SwitchVoiceGroupButton::onVoiceGroupChanged));

  Application::get().getPresetManager()->getEditBuffer()->onPresetLoaded(
      sigc::mem_fun(this, &SwitchVoiceGroupButton::rebuild));
}

void SwitchVoiceGroupButton::rebuild()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto selected = eb->getSelected(getHWUI()->getCurrentVoiceGroup());

  if(allowToggling(selected, eb))
    setText(StringAndSuffix { "I / II", 0 });
  else
    setText(StringAndSuffix { "", 0 });
}

void SwitchVoiceGroupButton::onParameterSelectionChanged(Parameter* oldSelected, Parameter* newSelection)
{
  rebuild();
}

void SwitchVoiceGroupButton::onVoiceGroupChanged(VoiceGroup newVoiceGroup)
{
  rebuild();
}

bool SwitchVoiceGroupButton::allowToggling(const Parameter* selected, const EditBuffer* editBuffer)
{
  if(selected == nullptr)
    return false;

  if(selected->getVoiceGroup() == VoiceGroup::Global)
    return false;

  if(editBuffer->getType() == SoundType::Single)
    return false;

  auto layerAndGroupAllowToggling
      = ((editBuffer->getType() == SoundType::Layer)
         && (!MonoGroup::isMonoParameter(selected) && !UnisonGroup::isUnisonParameter(selected)))
      || (editBuffer->getType() != SoundType::Layer);

  if(!selected->isDisabled())
    return layerAndGroupAllowToggling;
  else
    return false;
}
