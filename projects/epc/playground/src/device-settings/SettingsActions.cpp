#include "SettingsActions.h"
#include "Settings.h"
#include "Setting.h"
#include "SyncSplitSettingUseCases.h"
#include <device-settings/DebugLevel.h>
#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <http/NetworkRequest.h>
#include <proxies/hwui/TestLayout.h>

SettingsActions::SettingsActions(Settings &settings)
    : super("/settings/")
{
  addAction("set-setting", [&](std::shared_ptr<NetworkRequest> request) {
    Glib::ustring key = request->get("key");
    Glib::ustring value = request->get("value");

    DebugLevel::warning("Setting: ", key, " changed to: ", value);

    if(key == "SyncSplit")
    {
      auto useCases = SyncSplitSettingUseCases::get();
      useCases.updateFromWebUI(value);
      return;
    }

    if(auto s = settings.getSetting(key))
    {
      s->setSetting(Initiator::EXPLICIT_WEBUI, value);
    }
  });

  addAction("inc-test-display", [&](std::shared_ptr<NetworkRequest> request) {
    auto &app = Application::get();
    auto &boled = app.getHWUI()->getPanelUnit().getEditPanel().getBoled();
    auto &soled = app.getHWUI()->getBaseUnit().getPlayPanel().getSOLED();
    auto panelTestLayout = dynamic_cast<TestLayout *>(boled.getLayout().get());
    auto baseTestLayout = dynamic_cast<TestLayout *>(soled.getLayout().get());

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

SettingsActions::~SettingsActions()
{
}
