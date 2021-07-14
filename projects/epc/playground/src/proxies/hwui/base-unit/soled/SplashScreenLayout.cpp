#include <Application.h>
#include <proxies/hwui/base-unit/BaseUnit.h>
#include <proxies/hwui/base-unit/PlayPanel.h>
#include <proxies/hwui/base-unit/soled/SplashScreenLayout.h>
#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/HWUI.h>

SplashScreenLayout::SplashScreenLayout()
    : super(Application::get().getHWUI()->getBaseUnit().getPlayPanel().getSOLED())
{
  auto height = 32;
  auto labelHeight = 9;
  auto y = (height - labelHeight) / 2;
  addControl(new Label(StringAndSuffix { "Nonlinear Labs - C15" }, Rect(0, y, 128, labelHeight)));
}

SplashScreenLayout::~SplashScreenLayout() = default;
