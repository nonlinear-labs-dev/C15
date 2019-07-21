#include <nltools/threading/ContextBoundMessageQueue.h>

namespace nltools
{
  namespace threading
  {
    ContextBoundMessageQueue::ContextBoundMessageQueue(Glib::RefPtr<Glib::MainContext> context)
        : m_context(context)
    {
    }

    void ContextBoundMessageQueue::pushMessage(tMessage&& m)
    {
      std::lock_guard<std::recursive_mutex> l(m_mutex);
      m_queue.push_back(std::move(m));
      m_context->signal_idle().connect(sigc::mem_fun(this, &ContextBoundMessageQueue::doQueuedStuff));
    }

    bool ContextBoundMessageQueue::doQueuedStuff()
    {
      std::lock_guard<std::recursive_mutex> l(m_mutex);

      if(!m_queue.empty())
      {
        auto& m = m_queue.front();
        m();
        m_queue.pop_front();
      }

      return !m_queue.empty();
    }
  }
}
