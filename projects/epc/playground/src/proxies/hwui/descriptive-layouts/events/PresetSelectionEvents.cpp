#include "PresetSelectionEvents.h"
#include "proxies/hwui/descriptive-layouts/primitives/Text.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/Bank.h"
#include "presets/Preset.h"
#include "event-sources/base/EventSource.h"
#include "GlobalEventSinkBroker.h"
#include "GlobalEventSourceBroker.h"
#include "presets/EditBuffer.h"
#include <proxies/hwui/HWUI.h>

namespace DescriptiveLayouts
{

  PresetSelectionEvents::PresetSelectionEvents(const PresetManagerCursor::Matcher &filter)
      : m_cursor(filter)
  {
    createEventSources<Text::DisplayString>(
        EventSources::PreviousNumber, EventSources::PreviousName, EventSources::CurrentNumber,
        EventSources::CurrentName, EventSources::NextNumber, EventSources::NextName, EventSources::PresetListBankName);
    createEventSources<bool>(EventSources::CanLeft, EventSources::CanRight);

    m_cursor.moveToSelected();
    bruteForce();
  }

  PresetSelectionEvents::~PresetSelectionEvents() = default;

  sigc::connection PresetSelectionEvents::connect(EventSources source, const Callback &cb)
  {
    return eventSources.at(source)->connect(cb);
  }

  void PresetSelectionEvents::fire(EventSinks e)
  {
    switch(e)
    {
      case EventSinks::Left:
        if(m_cursor.canPreviousBank())
          m_cursor.previousBank();
        break;

      case EventSinks::Right:
        if(m_cursor.canNextBank())
          m_cursor.nextBank();
        break;

      case EventSinks::Up:
        if(m_cursor.canPreviousPreset())
          m_cursor.previousPreset();
        break;

      case EventSinks::Down:
        if(m_cursor.canNextPreset())
          m_cursor.nextPreset();
        break;

      case EventSinks::Commit:
        if(auto preset = m_cursor.getPreset())
        {
          auto eb = Application::get().getPresetManager()->getEditBuffer();
          EditBufferUseCases useCase(eb);
          auto currentVG = Application::get().getHWUI()->getCurrentVoiceGroup();
          useCase.undoableLoadToPart(preset, VoiceGroup::I, currentVG);
          Application::get().getHWUI()->setFocusAndMode(FocusAndMode(UIFocus::Sound, UIMode::Select, UIDetail::Init));
        }
        break;

      default:
        getGlobalEventSinkBroker().fire(e);
    }

    lastSeenPresetManagerRevision = -1;
    bruteForce();
  }

  void PresetSelectionEvents::bruteForce()
  {
    auto pm = Application::get().getPresetManager();
    auto updateId = pm->getUpdateIDOfLastChange();

    if(std::exchange(lastSeenPresetManagerRevision, updateId) != updateId)
    {
      connections.clear();

      auto currentCursor = m_cursor;
      auto previousCursor = currentCursor;
      previousCursor.previousPreset();
      auto nextCursor = currentCursor;
      nextCursor.nextPreset();

      setString(EventSources::PreviousNumber, previousCursor.getPresetNumberString());
      setString(EventSources::PreviousName, previousCursor.getPresetName());
      setString(EventSources::PreviousType, previousCursor.getPresetType());
      setString(EventSources::CurrentNumber, currentCursor.getPresetNumberString());
      setString(EventSources::CurrentName, currentCursor.getPresetName());
      setString(EventSources::CurrentType, currentCursor.getPresetType());
      setString(EventSources::NextNumber, nextCursor.getPresetNumberString());
      setString(EventSources::NextName, nextCursor.getPresetName());
      setString(EventSources::NextType, nextCursor.getPresetType());

      setString(EventSources::PresetListBankName, currentCursor.getBankName());

      static_cast<EventSource<bool> *>(eventSources.at(EventSources::CanLeft).get())
          ->setValue(currentCursor.canPreviousBank());
      static_cast<EventSource<bool> *>(eventSources.at(EventSources::CanRight).get())
          ->setValue(currentCursor.canNextBank());

      if(auto b = currentCursor.getBank())
        connections.push_back(b->onBankChanged(sigc::mem_fun(this, &PresetSelectionEvents::bruteForce)));

      if(auto p = previousCursor.getPreset())
        connections.push_back(p->onChanged(sigc::mem_fun(this, &PresetSelectionEvents::bruteForce)));

      if(auto p = currentCursor.getPreset())
        connections.push_back(p->onChanged(sigc::mem_fun(this, &PresetSelectionEvents::bruteForce)));

      if(auto p = nextCursor.getPreset())
        connections.push_back(p->onChanged(sigc::mem_fun(this, &PresetSelectionEvents::bruteForce)));
    }
  }

  void PresetSelectionEvents::setString(EventSources e, const std::string &str)
  {
    static_cast<EventSource<Text::DisplayString> *>(eventSources.at(e).get())->setValue({ str, 0 });
  }

  template <typename T, typename... P> void PresetSelectionEvents::createEventSources(P... e)
  {
    using D = EventSource<T>;
    std::initializer_list<bool>({ (eventSources[e] = std::make_unique<D>(), false)... });
  }
}
