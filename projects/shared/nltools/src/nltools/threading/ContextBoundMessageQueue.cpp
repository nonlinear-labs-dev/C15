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
      m_jobs.remove_if([&](const auto &a) { return a.unique(); }); //check if changed on staging

      m_pendingCalls++;

      auto job = std::make_shared<Job>([this, m = std::move(m)]() {
        m_pendingCalls--;
        m();
      });

      m_jobs.push_back(job);

      m_context->invoke([jobCopy = job] {
        auto closureIsOnlyOwner = jobCopy.unique();
        auto owningQueueStillExists = !closureIsOnlyOwner;

        if(owningQueueStillExists)
          (*jobCopy)();

        return false;
      });
    }

    bool ContextBoundMessageQueue::isPending() const
    {
      return m_pendingCalls > 0;
    }
  }
}
