#include <utility>

#include <nltools/threading/ContextBoundMessageQueue.h>
#include <nltools/logging/Log.h>

namespace nltools
{
  namespace threading
  {
    ContextBoundMessageQueue::ContextBoundMessageQueue(Glib::RefPtr<Glib::MainContext> context)
        : m_context(std::move(context))
    {
    }

    ContextBoundMessageQueue::~ContextBoundMessageQueue()
    {
      if(auto s = std::exchange(m_source, nullptr))
      {
        g_source_destroy(s);
        g_source_unref(s);
      }
    }

    void ContextBoundMessageQueue::pushMessage(tMessage &&m)
    {
      std::lock_guard<std::recursive_mutex> l(m_mutex);
      m_queue.push_back(std::move(m));

      if(!m_source)
      {
        m_source = g_idle_source_new();
        g_source_set_callback(m_source, reinterpret_cast<GSourceFunc>(+[](gpointer u) {
                                auto pThis = reinterpret_cast<ContextBoundMessageQueue *>(u);
                                return pThis->doQueuedStuff();
                              }),
                              this, nullptr);
        g_source_attach(m_source, m_context->gobj());
      }
    }

    bool ContextBoundMessageQueue::doQueuedStuff()
    {
      std::lock_guard<std::recursive_mutex> l(m_mutex);

      if(!m_queue.empty())
      {
        auto m = std::move(m_queue.front());
        m_queue.pop_front();
        m();
      }

      if(m_queue.empty())
      {
        g_source_destroy(m_source);
        g_source_unref(m_source);
        m_source = nullptr;
        return false;
      }

      return true;
    }
  }
}
