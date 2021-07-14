#pragma once

#include "proxies/hwui/descriptive-layouts/events/event-sources/base/EventSource.h"

class Parameter;
class ParameterGroup;

namespace DescriptiveLayouts
{
  template <typename T> class ParameterEvent : public EventSource<T>
  {
   public:
    explicit ParameterEvent();
    ~ParameterEvent() override;

    virtual void onParameterChanged(const Parameter *p);
    virtual void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam);
    virtual void onSelectedParameterChanged(const Parameter *p) = 0;
    virtual void onGroupSelectionChanged(const ParameterGroup *group);

   protected:
    sigc::connection m_onParameterChangedConnection;
    sigc::connection m_onParameterSelectionChangedConnection;
  };

  extern template class ParameterEvent<bool>;
  extern template class ParameterEvent<DisplayString>;
  extern template class ParameterEvent<tControlPositionValue>;
  extern template class ParameterEvent<Range>;

}
