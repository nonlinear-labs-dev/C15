#include "SpecialParameterCarousel.h"
#include "NeverHighlitButton.h"
#include "MiniParameter.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <parameters/mono-mode-parameters/ModulateableMonoParameter.h>
#include <parameters/mono-mode-parameters/UnmodulateableMonoParameter.h>
#include <groups/MonoGroup.h>
#include <sigc++/sigc++.h>

SpecialParameterCarousel::SpecialParameterCarousel(const Rect &rect)
    : ParameterCarousel(rect)
{
  m_editbufferConnection = Application::get().getPresetManager()->getEditBuffer()->onChange(
      sigc::mem_fun(this, &SpecialParameterCarousel::rebuild));

  m_voiceGroupConnection = Application::get().getHWUI()->onCurrentVoiceGroupChanged(
      sigc::hide(sigc::mem_fun(this, &SpecialParameterCarousel::rebuild)));
}

void SpecialParameterCarousel::setup(Parameter *selectedParameter)
{
  clear();

  setupControls(selectedParameter);

  if(getNumChildren() == 0)
  {
    addControl(new NeverHighlitButton("", Rect(0, 51, 58, 11)));
  }
  else
  {
    setHighlight(true);
  }

  setDirty();
}

void SpecialParameterCarousel::rebuild()
{
  auto s = Application::get().getPresetManager()->getEditBuffer()->getSelected(getHWUI()->getCurrentVoiceGroup());
  setup(s);
}

SpecialParameterCarousel::~SpecialParameterCarousel()
{
  m_editbufferConnection.disconnect();
  m_voiceGroupConnection.disconnect();
}

void SpecialParameterCarousel::setupControls(Parameter *parameter)
{
  const auto ySpaceing = 3;
  const int miniParamHeight = 12;
  const int miniParamWidth = 56;
  int yPos = ySpaceing;

  const auto soundType = Application::get().getPresetManager()->getEditBuffer()->getType();
  for(const auto &id : getParameterIdsForMode(soundType))
  {
    auto param = Application::get().getPresetManager()->getEditBuffer()->findParameterByID(id);
    auto miniParam = new MiniParameter(param, Rect(0, yPos, miniParamWidth, miniParamHeight));
    miniParam->setSelected(param == parameter);
    addControl(miniParam);
    yPos += ySpaceing;
    yPos += miniParamHeight;
  }
}
