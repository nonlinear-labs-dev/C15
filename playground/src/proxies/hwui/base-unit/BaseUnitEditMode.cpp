#include "BaseUnitEditMode.h"
#include "Application.h"
#include "device-settings/Settings.h"
#include "device-settings/ParameterEditModeRibbonBehaviour.h"
#include "proxies/lpc/LPCProxy.h"
#include "groups/HardwareSourcesGroup.h"

BaseUnitEditMode::BaseUnitEditMode()
{
}

BaseUnitEditMode::~BaseUnitEditMode()
{
}

void BaseUnitEditMode::toggleTouchBehaviour()
{
  if(Application::get().getLPCProxy()->getLastTouchedRibbonParameterID()
     == HardwareSourcesGroup::getUpperRibbonParameterID())
  {
    toggleParameterEditModeRibbonBehaviour();
  }
  else
  {
    super::toggleTouchBehaviour();
  }
}

void BaseUnitEditMode::toggleParameterEditModeRibbonBehaviour()
{
  Application::get().getSettings()->getSetting<ParameterEditModeRibbonBehaviour>()->inc();
}
