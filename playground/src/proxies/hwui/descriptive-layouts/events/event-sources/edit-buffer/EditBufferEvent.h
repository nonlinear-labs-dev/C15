#pragma once

#include <presets/EditBuffer.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/HWUI.h>

#include "../base/EventSource.h"

namespace DescriptiveLayouts
{
  struct EventMerger : sigc::trackable
  {
    EventMerger(EditBuffer* eb, HWUI* hwui)
    {
      m_changedConnection = eb->onChange(sigc::mem_fun(this, &EventMerger::onChange));
      m_presetLoadedConnection = eb->onPresetLoaded(sigc::mem_fun(this, &EventMerger::onChange));
      m_onRecallChanged = eb->onRecallValuesChanged(sigc::mem_fun(this, &EventMerger::onChange));
      m_onHardwareUIVoiceGroupSelectionChanged
          = hwui->onCurrentVoiceGroupChanged(sigc::hide(sigc::mem_fun(this, &EventMerger::onChange)));
    }

    void onChange()
    {
      if(!std::exchange(m_scheduled, true))
      {
        m_throttler.doTask([this] {
          m_scheduled = false;
          this->m_signal.send();
        });
      }
    }

    template <typename CB> sigc::connection connect(CB&& c)
    {
      return m_signal.connect(std::move(c));
    }

    bool m_scheduled = false;
    sigc::connection m_changedConnection;
    sigc::connection m_presetLoadedConnection;
    sigc::connection m_onRecallChanged;
    sigc::connection m_onHardwareUIVoiceGroupSelectionChanged;
    Signal<void> m_signal;
    Throttler m_throttler{ std::chrono::milliseconds(1) };
  };

  EventMerger& getTheEventMerger(EditBuffer* eb, HWUI* hwui);

  template <typename T> class EditBufferEvent : public EventSource<T>
  {
   public:
    EditBufferEvent()
    {
      m_changedConnection = getTheEventMerger(getEditBuffer(), Application::get().getHWUI()).connect([this] {
        this->onChange(getEditBuffer());
      });
    }

    ~EditBufferEvent()
    {
      m_changedConnection.disconnect();
    }

    virtual void onChange(const EditBuffer* eb) = 0;

    static EditBuffer* getEditBuffer()
    {
      return Application::get().getPresetManager()->getEditBuffer();
    }

   private:
    sigc::connection m_changedConnection;
  };
}
