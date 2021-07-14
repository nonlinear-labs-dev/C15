#pragma once

#include "ParameterEvent.h"
#include <proxies/hwui/HWUIEnums.h>

namespace DescriptiveLayouts
{
  class ParameterGroupNameEventSource : public ParameterEvent<DisplayString>
  {
   public:
    void onSelectedParameterChanged(const Parameter *p) override;
  };

  class ParameterIsBipolarEventSource : public ParameterEvent<bool>
  {
   public:
    void onSelectedParameterChanged(const Parameter *p) override;
  };

  class CurrentParameterControlPosition : public ParameterEvent<tControlPositionValue>
  {
   public:
    void onSelectedParameterChanged(const Parameter *p) override;
  };

  class ParameterNameEventSource : public ParameterEvent<DisplayString>
  {
    void onSelectedParameterChanged(const Parameter *p) override;
  };

  class ParameterNameWithStateSuffixEventSource : public ParameterEvent<DisplayString>
  {
   public:
    void onSelectedParameterChanged(const Parameter *parameter) override;
  };

  class ParameterDisplayStringEventSource : public ParameterEvent<DisplayString>
  {
   public:
    explicit ParameterDisplayStringEventSource();
    ~ParameterDisplayStringEventSource() override;

    void onSelectedParameterChanged(const Parameter *p) override;

   private:
    void onModifierChanged(::ButtonModifiers mods);

   private:
    sigc::connection m_modifierConnection;
  };

  class ParameterValueChanged : public ParameterEvent<bool>
  {
   public:
    void onSelectedParameterChanged(const Parameter *p) override;
  };
}
