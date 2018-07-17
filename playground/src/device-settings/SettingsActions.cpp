#include "SettingsActions.h"
#include "Settings.h"
#include "Setting.h"
#include "BenderCurve.h"
#include <device-settings/DebugLevel.h>
#include <Application.h>
#include <proxies/hwui/TestLayout.h>
#include <proxies/hwui/HWUI.h>

SettingsActions::SettingsActions (Settings &settings) :
    super ("/settings/")
{
  addAction ("set-setting", [&](shared_ptr<NetworkRequest> request)
  {
    Glib::ustring key = request->get("key");
    Glib::ustring value = request->get("value");

    DebugLevel::warning("Setting: ", key, " changed to: ", value);

    if(auto s = settings.getSetting (key))
    {
      s->setSetting(Initiator::EXPLICIT_WEBUI, value);
    }
  });

  addAction("set-bender-raw", [&](shared_ptr<NetworkRequest> request)
  {
    if(auto benderSetting = dynamic_pointer_cast<BenderCurve>(settings.getSetting("BenderCurve")))
    {
      benderSetting->setRawMode(!(request->get("rawSetting", "false") == "false"));
    }
  });

  addAction("inc-test-display", [&](shared_ptr<NetworkRequest> request)
  {
    auto &app = Application::get();
        auto &boled = app.getHWUI()->getPanelUnit().getEditPanel().getBoled();
        auto &soled = app.getHWUI()->getBaseUnit().getPlayPanel().getSOLED();
        auto panelTestLayout = dynamic_pointer_cast<TestLayout>(boled.getLayout());
        auto baseTestLayout = dynamic_pointer_cast<TestLayout>(soled.getLayout());

    if(panelTestLayout && baseTestLayout)
    {
      baseTestLayout->iterate();
      panelTestLayout->iterate();
    }
    else
    {
      boled.setOverlay(new TestLayout(boled));
      soled.setOverlay(new TestLayout(soled));
    }
  });
}

SettingsActions::~SettingsActions ()
{
}

