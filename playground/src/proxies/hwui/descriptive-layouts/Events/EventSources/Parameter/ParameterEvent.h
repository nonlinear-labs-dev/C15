#pragma once
#include <parameters/Parameter.h>
#include "proxies/hwui/descriptive-layouts/Events/EventSources/base/EventSource.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>

namespace DescriptiveLayouts
{
  template <typename T> class ParameterEvent : public EventSource<T>
  {
   public:
    explicit ParameterEvent();
    ~ParameterEvent();

    virtual void onParameterChanged(const Parameter *p);
    virtual void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam);
    virtual void onSelectedParameterChanged(const Parameter *p) = 0;
    virtual void onGroupSelectionChanged(const ParameterGroup* group);

   protected:
    sigc::connection m_onParameterChangedConnection;
    sigc::connection m_onParameterSelectionChangedConnection;
  };

  template <typename T> ParameterEvent<T>::ParameterEvent()
  {
    m_onParameterSelectionChangedConnection
        = Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
            sigc::mem_fun(this, &ParameterEvent::onParameterSelectionChanged));
  }

  template <typename T> ParameterEvent<T>::~ParameterEvent()
  {
    m_onParameterSelectionChangedConnection.disconnect();
    m_onParameterChangedConnection.disconnect();
  }

  template <typename T> void ParameterEvent<T>::onParameterChanged(const Parameter *p)
  {
    onSelectedParameterChanged(p);
  }

  template <typename T>
  void ParameterEvent<T>::onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
  {
    m_onParameterChangedConnection.disconnect();
    if(newParam) {
      if(oldParam && newParam->getParentGroup() != oldParam->getParentGroup())
        onGroupSelectionChanged(newParam->getParentGroup());

      m_onParameterChangedConnection = newParam->onParameterChanged(sigc::mem_fun(this, &ParameterEvent<T>::onParameterChanged));
    }
  }

  template<typename T>
  void ParameterEvent<T>::onGroupSelectionChanged(const ParameterGroup *group) {
  }
}
