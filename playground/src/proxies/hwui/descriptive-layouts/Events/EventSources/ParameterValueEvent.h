#pragma once
#include <parameters/Parameter.h>
#include "base/EventSource.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>

namespace DescriptiveLayouts
{
  template <typename T> class ParameterValueEvent : public EventSource<T>
  {
   public:
    explicit ParameterValueEvent();
    ~ParameterValueEvent();

    virtual void onParameterChanged(const Parameter *p);
    virtual void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam);
    virtual void onChange(const Parameter *p) = 0;

   protected:
    sigc::connection m_onParameterChangedConnection;
    sigc::connection m_onParameterSelectionChangedConnection;
  };

  template <typename T> ParameterValueEvent<T>::ParameterValueEvent()
  {
    m_onParameterSelectionChangedConnection
        = Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
            sigc::mem_fun(this, &ParameterValueEvent::onParameterSelectionChanged));
  }

  template <typename T> ParameterValueEvent<T>::~ParameterValueEvent()
  {
    m_onParameterSelectionChangedConnection.disconnect();
    m_onParameterChangedConnection.disconnect();
  }

  template <typename T> void ParameterValueEvent<T>::onParameterChanged(const Parameter *p)
  {
    onChange(p);
  }

  template <typename T>
  void ParameterValueEvent<T>::onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
  {
    m_onParameterChangedConnection.disconnect();
    if(newParam) {
      m_onParameterChangedConnection = newParam->onParameterChanged(sigc::mem_fun(this, &ParameterValueEvent<T>::onParameterChanged));
    }
  }
}
