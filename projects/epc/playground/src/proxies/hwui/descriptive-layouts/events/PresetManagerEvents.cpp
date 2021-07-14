#include <use-cases/DirectLoadUseCases.h>
#include "PresetManagerEvents.h"
#include "proxies/hwui/descriptive-layouts/primitives/Text.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/Bank.h"
#include "presets/Preset.h"
#include "presets/EditBuffer.h"
#include "event-sources/base/EventSource.h"
#include "GlobalEventSinkBroker.h"
#include "GlobalEventSourceBroker.h"
#include "presets/PresetManagerCursor.h"
#include "http/UndoScope.h"
#include "device-settings/Settings.h"
#include "device-settings/DirectLoadSetting.h"

namespace DescriptiveLayouts
{

  PresetManagerEvents::PresetManagerEvents()
      : m_throttler(std::chrono::milliseconds(10))
  {
    createEventSources<Text::DisplayString>(
        EventSources::PreviousNumber, EventSources::PreviousName, EventSources::CurrentNumber,
        EventSources::CurrentName, EventSources::NextNumber, EventSources::NextName, EventSources::PresetListBankName);

    createEventSources<bool>(EventSources::CanLeft, EventSources::CanRight, EventSources::PreviousSelected,
                             EventSources::PreviousLoaded, EventSources::CurrentSelected, EventSources::CurrentLoaded,
                             EventSources::NextSelected, EventSources::NextLoaded, EventSources::DirectLoadStatus);

    bruteForce();
  }

  PresetManagerEvents::~PresetManagerEvents() = default;

  sigc::connection PresetManagerEvents::connect(EventSources source, const Callback &cb)
  {
    return eventSources.at(source)->connect(cb);
  }

  void PresetManagerEvents::fire(EventSinks e)
  {
    PresetManagerCursor cursor;

    switch(e)
    {
      case EventSinks::ToggleDirectLoad:
      {
        auto setting = Application::get().getSettings()->getSetting<DirectLoadSetting>();
        DirectLoadUseCases useCase(setting.get());
        useCase.toggleDirectLoadFromHWUI(Application::get().getHWUI());
      }
      break;

      case EventSinks::Left:
        if(cursor.canPreviousBank())
          cursor.previousBank();
        break;

      case EventSinks::Right:
        if(cursor.canNextBank())
          cursor.nextBank();
        break;

      case EventSinks::Up:
        if(cursor.canPreviousPreset())
          cursor.previousPreset();
        break;

      case EventSinks::Down:
        if(cursor.canNextPreset())
          cursor.nextPreset();
        break;

      default:
        getGlobalEventSinkBroker().fire(e);
    }

    auto pm = Application::get().getPresetManager();
    PresetManagerUseCases useCases(pm);
    if(auto b = cursor.getBank())
    {
      auto currentBank = pm->getSelectedBank();

      if(currentBank != b)
      {
        useCases.selectBank(b);
      }
      else if(currentBank)
      {
        auto currentPreset = currentBank->getSelectedPreset();
        auto newPreset = cursor.getPreset();

        if(currentPreset != newPreset && newPreset)
        {
          useCases.selectPreset(newPreset);
        }
      }
    }

    lastSeenPresetManagerRevision = -1;
    bruteForce();
  }

  void PresetManagerEvents::bruteForce()
  {
    m_throttler.doTask(
        [this]
        {
          auto pm = Application::get().getPresetManager();
          auto updateId = pm->getUpdateIDOfLastChange();

          if(std::exchange(lastSeenPresetManagerRevision, updateId) != updateId)
          {
            connections.clear();

            auto selectedPresetUuid = Uuid::none();

            if(auto b = pm->getSelectedBank())
              selectedPresetUuid = b->getSelectedPresetUuid();

            auto loadedPresetUuid = pm->getEditBuffer()->getUUIDOfLastLoadedPreset();

            PresetManagerCursor currentCursor;

            auto previousCursor = currentCursor;
            previousCursor.previousPreset();
            auto nextCursor = currentCursor;
            nextCursor.nextPreset();

            setString(EventSources::PreviousNumber, previousCursor.getPresetNumberString());
            setString(EventSources::PreviousName, previousCursor.getPresetName());
            setString(EventSources::CurrentNumber, currentCursor.getPresetNumberString());
            setString(EventSources::CurrentName, currentCursor.getPresetName());
            setString(EventSources::NextNumber, nextCursor.getPresetNumberString());
            setString(EventSources::NextName, nextCursor.getPresetName());

            setString(EventSources::PresetListBankName, currentCursor.getBankName());

            setBool(EventSources::CanLeft, currentCursor.canPreviousBank());
            setBool(EventSources::CanRight, currentCursor.canNextBank());

            setBool(EventSources::PreviousSelected, previousCursor.getPresetUuid() == selectedPresetUuid);
            setBool(EventSources::PreviousLoaded, previousCursor.getPresetUuid() == loadedPresetUuid);
            setBool(EventSources::CurrentSelected, currentCursor.getPresetUuid() == selectedPresetUuid);
            setBool(EventSources::CurrentLoaded, currentCursor.getPresetUuid() == loadedPresetUuid);
            setBool(EventSources::NextSelected, nextCursor.getPresetUuid() == selectedPresetUuid);
            setBool(EventSources::NextLoaded, nextCursor.getPresetUuid() == loadedPresetUuid);

            auto bruteForce = sigc::mem_fun(this, &PresetManagerEvents::bruteForce);

            if(auto b = currentCursor.getBank())
              connections.push_back(b->onBankChanged(bruteForce));

            if(auto p = previousCursor.getPreset())
              connections.push_back(p->onChanged(bruteForce));

            if(auto p = currentCursor.getPreset())
              connections.push_back(p->onChanged(bruteForce));

            if(auto p = nextCursor.getPreset())
              connections.push_back(p->onChanged(bruteForce));

            connections.push_back(pm->onBankSelection(sigc::hide(bruteForce)));
            connections.push_back(pm->onRestoreHappened(bruteForce));
            connections.push_back(pm->getEditBuffer()->onPresetLoaded(bruteForce));
            connections.push_back(Application::get().getSettings()->getSetting<DirectLoadSetting>()->onChange(
                sigc::hide(sigc::mem_fun(this, &PresetManagerEvents::updateDirectLoad))));
          }
        });
  }

  void PresetManagerEvents::updateDirectLoad()
  {
    setBool(EventSources::DirectLoadStatus, Application::get().getSettings()->getSetting<DirectLoadSetting>()->get());
  }

  void PresetManagerEvents::setString(EventSources e, const std::string &str)
  {
    static_cast<EventSource<Text::DisplayString> *>(eventSources.at(e).get())->setValue({ str, 0 });
  }

  void PresetManagerEvents::setBool(EventSources e, bool b)
  {
    static_cast<EventSource<bool> *>(eventSources.at(e).get())->setValue(b);
  }

  template <typename T, typename... P> void PresetManagerEvents::createEventSources(P... e)
  {
    using D = EventSource<T>;
    std::initializer_list<bool>({ (eventSources[e] = std::make_unique<D>(), false)... });
  }
}
