#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterValueLabel.h>
#include <Application.h>
#include "presets/EditBuffer.h"
#include "presets/PresetManager.h"
#include "ParameterOverlay.h"
#include <libundo/undo/Scope.h>
#include <parameters/Parameter.h>

ParameterOverlay::ParameterOverlay(const Rect& rect, Parameter* const parameter)
    : ArrowIncrementDecrementOverlay(rect)
    , m_parameter { parameter }
{
  auto labelWidth = rect.getWidth() - 20;
  m_label = addControl(new ParameterValueLabel(parameter, { 11, -1, labelWidth, 13 }));
}

void ParameterOverlay::onLeft(bool down)
{
  ArrowIncrementDecrementOverlay::onLeft(down);
  ParameterUseCases useCase(m_parameter);
  useCase.incDec(-1, false, false);
}

void ParameterOverlay::onRight(bool down)
{
  ArrowIncrementDecrementOverlay::onRight(down);
  ParameterUseCases useCase(m_parameter);
  useCase.incDec(1, false, false);
}
