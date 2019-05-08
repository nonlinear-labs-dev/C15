#include "SelectedParameterBarSlider.h"
#include "Application.h"
#include "parameters/Parameter.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"

SelectedParameterBarSlider::SelectedParameterBarSlider(const Rect &rect)
    : super(rect)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      sigc::hide<0>(sigc::mem_fun(this, &SelectedParameterBarSlider::setParameter)));
}

SelectedParameterBarSlider::~SelectedParameterBarSlider()
{
}
