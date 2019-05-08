#include "SOLED.h"
#include "Application.h"
#include "proxies/hwui/Oleds.h"
#include "PlayModeLayout.h"
#include "EditModeLayout.h"
#include "SplashScreenLayout.h"
#include "BanksLayout.h"
#include "PresetsLayout.h"
#include "device-settings/Settings.h"
#include "device-settings/BaseUnitUIMode.h"
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/FrameBuffer.h>

SOLED::SOLED()
    : OLEDProxy(Rect(0, 64, 128, 32))
{
  Application::get().getSettings()->getSetting<BaseUnitUIMode>()->onChange(
      mem_fun(this, &SOLED::onBaseUnitUIModeChange));
}

SOLED::~SOLED()
{
}

void SOLED::init()
{
  reset(new SplashScreenLayout());
}

void SOLED::resetSplash()
{
  onBaseUnitUIModeChange(Application::get().getSettings()->getSetting<BaseUnitUIMode>().get());
}

void SOLED::onBaseUnitUIModeChange(const Setting *s)
{
  auto d = dynamic_cast<const BaseUnitUIMode *>(s);

  switch(d->get())
  {
    case BaseUnitUIModes::Play:
      reset(new PlayModeLayout());
      break;

    case BaseUnitUIModes::ParameterEdit:
      reset(new EditModeLayout());
      break;

    case BaseUnitUIModes::Banks:
      reset(new BanksLayout());
      break;

    case BaseUnitUIModes::Presets:
      reset(new PresetsLayout());
      break;
  }
}
