#pragma once

#include "EventSource.h"

class Parameter;

namespace DescriptiveLayouts {
  class GenericParameterDisplayValueEvent : public EventSource<DisplayString> {
  public:
    explicit GenericParameterDisplayValueEvent();
    ~GenericParameterDisplayValueEvent();

    virtual void onParameterChanged(const Parameter *p);
    virtual void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam);
    virtual void onChange(const Parameter* p) = 0;

  protected:
    sigc::connection m_onParameterChangedConnection;
    sigc::connection m_onParameterSelectionChangedConnection;
  };
}