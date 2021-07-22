#include "SettingsActions.h"
#include "Settings.h"
#include "SyncSplitSettingUseCases.h"
#include <device-settings/DebugLevel.h>
#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <http/NetworkRequest.h>
#include <proxies/hwui/TestLayout.h>
#include <use-cases/SettingsUseCases.h>
#include <tools/ExceptionTools.h>
#include <use-cases/DirectLoadUseCases.h>
#include <presets/PresetManager.h>

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

  addAction("set-direct-load-with-load-to-part", [&](const std::shared_ptr<NetworkRequest>& request) {
    auto pm = Application::get().getPresetManager();
    auto state = request->get("state");
    auto presetIfInLoadToPart = request->get("preset");
    auto fromIfInLoadToPart = request->get("from");
    auto totIfInLoadToPart = request->get("to");

    DirectLoadUseCases useCase(settings.getSetting<DirectLoadSetting>().get());

    try {
      if(state == "on") {
        if(auto preset = pm->findPreset(Uuid{presetIfInLoadToPart})) {
          useCase.enableDirectLoadFromWebUI(preset, to<VoiceGroup>(fromIfInLoadToPart), to<VoiceGroup>(totIfInLoadToPart));
        } else {
          useCase.enableDirectLoadWithoutPreset();
        }
      } else if(state == "off") {
        useCase.disableDirectLoad();
      }
    } catch(const std::runtime_error& e) {
      nltools::Log::error("Catched Error in \"set-direct-load-with-load-to-part\":", e.what());
    }
  });

  addAction("set-direct-load-without-load-to-part", [&](const std::shared_ptr<NetworkRequest>& request) {
    DirectLoadUseCases useCase(settings.getSetting<DirectLoadSetting>().get());
    auto state = request->get("state") == "on";
    useCase.setDirectLoad(state);
  });

  addAction("default-high-res", [&](auto request) {
    SettingsUseCases useCase(Application::get().getSettings());
    useCase.setMappingsToHighRes();
  });

  addAction("default-classic-midi", [&](auto request) {
    SettingsUseCases useCase(Application::get().getSettings());
    useCase.setMappingsToClassicMidi();
  });

  addAction("set-routing-aspect", [&](auto request) {
    try
    {
      auto hw = std::stoi(request->get("routing-entry"));
      auto aspect = std::stoi(request->get("aspect"));
      auto value = request->get("value") == "1";

      SettingsUseCases useCase(Application::get().getSettings());
      useCase.updateRoutingAspect(hw, aspect, value);
    }
    catch(...)
    {
      ExceptionTools::errorLogCurrentException();
    }
   });

  addAction("panic-audio-engine", [](auto request) {
    SettingsUseCases::panicAudioEngine();
  });

  addAction("set-all-routings-to-value", [&](auto request) {
       auto requestedState = request->get("state") == "1";
       SettingsUseCases useCase(Application::get().getSettings());
       useCase.setAllRoutingEntries(requestedState);
       nltools::Log::error(ExceptionTools::handle_eptr(std::current_exception()));
  });
}

SettingsActions::~SettingsActions() = default;
