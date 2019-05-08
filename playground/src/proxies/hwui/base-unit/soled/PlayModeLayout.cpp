#include "Application.h"
#include "groups/HardwareSourcesGroup.h"
#include "PlayModeLayout.h"
#include "RibbonLabel.h"
#include "PlayModeRibbonBehaviourLabel.h"
#include "MCRoutings.h"

PlayModeLayout::PlayModeLayout()
    : super()
{
  addControl(new RibbonLabel(HardwareSourcesGroup::getUpperRibbonParameterID(), Rect(25, 1, 74, 14)));
  addControl(new MCRoutings(HardwareSourcesGroup::getUpperRibbonParameterID(), Rect(99, 4, 8, 8)));

  addControl(new PlayModeRibbonBehaviourLabel(HardwareSourcesGroup::getUpperRibbonParameterID(),
                                              Rect(getBehaviourLeft(), 1, 12, 14)));
}

PlayModeLayout::~PlayModeLayout()
{
}
