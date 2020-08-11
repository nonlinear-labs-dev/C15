#include "ParameterEvent.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/Parameter.h>
#include <proxies/hwui/HWUI.h>

namespace DescriptiveLayouts
{
  template <typename T> ParameterEvent<T>::ParameterEvent()
  {
    m_onParameterSelectionChangedConnection
        = Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
            sigc::mem_fun(this, &ParameterEvent::onParameterSelectionChanged),
            Application::get().getHWUI()->getCurrentVoiceGroup());
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

  template <typename T> void ParameterEvent<T>::onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
  {
    m_onParameterChangedConnection.disconnect();
    if(newParam)
    {
      if(oldParam && newParam->getParentGroup() != oldParam->getParentGroup())
        onGroupSelectionChanged(newParam->getParentGroup());

      m_onParameterChangedConnection
          = newParam->onParameterChanged(sigc::mem_fun(this, &ParameterEvent<T>::onParameterChanged));
    }
  }

  template <typename T> void ParameterEvent<T>::onGroupSelectionChanged(const ParameterGroup *group)
  {
  }

  template class ParameterEvent<bool>;
  template class ParameterEvent<DisplayString>;
  template class ParameterEvent<tControlPositionValue>;
  template class ParameterEvent<Range>;
}
