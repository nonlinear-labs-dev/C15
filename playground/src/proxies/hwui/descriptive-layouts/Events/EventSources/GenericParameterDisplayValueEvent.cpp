#include "GenericParameterDisplayValueEvent.h"
#include <Application.h>
#include <parameters/Parameter.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>

namespace DescriptiveLayouts {
  GenericParameterDisplayValueEvent::GenericParameterDisplayValueEvent() {
      m_onParameterSelectionChangedConnection = Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
          sigc::mem_fun(this, &GenericParameterDisplayValueEvent::onParameterSelectionChanged));
  }

  GenericParameterDisplayValueEvent::~GenericParameterDisplayValueEvent() {
    m_onParameterSelectionChangedConnection.disconnect();
    m_onParameterChangedConnection.disconnect();
  }

  void GenericParameterDisplayValueEvent::onParameterChanged(const Parameter *p) {
    onChange(p);
  }

  void GenericParameterDisplayValueEvent::onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam) {
    m_onParameterChangedConnection.disconnect();
    if(newParam) {
      m_onParameterChangedConnection = newParam->onParameterChanged(sigc::mem_fun(this, &GenericParameterDisplayValueEvent::onParameterChanged));
    }
  }

}