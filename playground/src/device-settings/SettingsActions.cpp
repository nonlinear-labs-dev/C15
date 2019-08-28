#include "SettingsActions.h"
#include "Settings.h"
#include "Setting.h"
#include <device-settings/DebugLevel.h>
#include <Application.h>
#include <proxies/hwui/HWUI.h>

SettingsActions::SettingsActions(Settings &settings)
    : super("/settings/")
{
  addAction("set-setting", [&](std::shared_ptr<NetworkRequest> request) {
    Glib::ustring key = request->get("key");
    Glib::ustring value = request->get("value");

    DebugLevel::warning("Setting: ", key, " changed to: ", value);

    if(auto s = settings.getSetting(key))
    {
      s->setSetting(Initiator::EXPLICIT_WEBUI, value);
    }
  });
}

SettingsActions::~SettingsActions()
{
}
