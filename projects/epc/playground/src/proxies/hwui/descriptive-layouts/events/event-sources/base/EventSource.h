#pragma once
#include "EventSourceBase.h"

namespace DescriptiveLayouts
{
  using DisplayString = std::pair<Glib::ustring, int>;
  using Range = std::pair<tControlPositionValue, tControlPositionValue>;

  template <typename T> class EventSource : public EventSourceBase
  {
   public:
    EventSource() = default;

    virtual void setValue(const T &v)
    {
      set_as(v);
    }

   private:
    template <class TT> void set_as(const TT &v)
    {
      if(v != m_lastValue)
      {
        m_lastValue = v;
        m_outputSignal.send(m_lastValue);
      }
    }

    void set_as(const DisplayString &v)
    {
      auto hasher = std::hash<std::string>();
      const auto hashNew = hasher(v.first);
      const auto hashOld = hasher(m_lastValue.first);

      if(v.first != m_lastValue.first || hashOld != hashNew)
      {
        m_lastValue = v;
        m_outputSignal.send(m_lastValue);
      }
    }

   protected:
    std::experimental::any getLastValue() const override
    {
      return m_lastValue;
    }

    T m_lastValue {};
  };
}
