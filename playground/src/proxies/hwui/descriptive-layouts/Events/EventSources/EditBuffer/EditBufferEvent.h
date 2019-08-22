#pragma once

#include <presets/EditBuffer.h>
#include <Application.h>
#include <presets/PresetManager.h>

#include "../base/EventSource.h"

namespace DescriptiveLayouts
{
  template <typename T> class EditBufferEvent : public EventSource<T>
  {
   public:
    EditBufferEvent()
    {
      auto eb = getEditBuffer();
      m_changedConnection = eb->onChange(sigc::mem_fun(this, &EditBufferEvent<T>::onEditBufferChanged));
      m_presetLoadedConnection = eb->onPresetLoaded(sigc::mem_fun(this, &EditBufferEvent<T>::onPresetLoaded));
      m_onRecallChanged = eb->onRecallValuesChanged(sigc::mem_fun(this, &EditBufferEvent<T>::onRecallChanged));
    }

    ~EditBufferEvent()
    {
      m_changedConnection.disconnect();
      m_presetLoadedConnection.disconnect();
      m_onRecallChanged.disconnect();
    }

    virtual void onEditBufferChanged()
    {
      onChange(getEditBuffer());
    }

    virtual void onPresetLoaded()
    {
      onChange(getEditBuffer());
    }

    virtual void onRecallChanged()
    {
      onChange(getEditBuffer());
    }

    virtual void onChange(const EditBuffer* eb) = 0;

    static EditBuffer* getEditBuffer()
    {
      return Application::get().getPresetManager()->getEditBuffer();
    }

   private:
    sigc::connection m_changedConnection;
    sigc::connection m_presetLoadedConnection;
    sigc::connection m_onRecallChanged;
  };
}