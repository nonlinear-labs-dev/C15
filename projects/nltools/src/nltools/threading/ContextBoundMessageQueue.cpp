#include <nltools/threading/ContextBoundMessageQueue.h>
#include <nltools/logging/Log.h>
#include <glibmm.h>

namespace nltools
{
  namespace threading
  {
    ContextBoundMessageQueue::ContextBoundMessageQueue(Glib::RefPtr<Glib::MainContext> context)
        : m_context(std::move(context))
    {
    }

    ContextBoundMessageQueue::~ContextBoundMessageQueue() = default;

    void ContextBoundMessageQueue::pushMessage(tMessage &&m)
    {
      m_pendingCalls++;

      m_context->invoke([this, m = std::move(m)]() {
        m_pendingCalls--;
        m();
        return false;
      });
    }

    bool ContextBoundMessageQueue::isPending() const
    {
      return m_pendingCalls > 0;
    }
  }
}
