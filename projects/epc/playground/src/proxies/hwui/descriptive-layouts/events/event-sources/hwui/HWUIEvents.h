#pragma once
#include "../base/EventSource.h"
#include "Application.h"
#include "proxies/hwui/HWUI.h"

namespace DescriptiveLayouts::HWUIEvents
{
  template <typename T> class HWUIEvent : public EventSource<T>
  {
   public:
    explicit HWUIEvent();
    ~HWUIEvent() override;

    virtual void onButtonModifiersChanged(const ButtonModifiers &mod) = 0;

   protected:
    sigc::connection m_onModifiersChanged;
  };

  template <typename T> HWUIEvent<T>::HWUIEvent()
  {
    m_onModifiersChanged
        = Application::get().getHWUI()->onModifiersChanged(sigc::mem_fun(this, &HWUIEvent::onButtonModifiersChanged));
  }

  template <typename T> HWUIEvent<T>::~HWUIEvent()
  {
    m_onModifiersChanged.disconnect();
  }

  class isFineEventSource : public HWUIEvent<bool>
  {
   public:
    void onButtonModifiersChanged(const ButtonModifiers &mod) override;
  };
}