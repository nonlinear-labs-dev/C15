#pragma once

#include <glibmm/main.h>
#include <functional>
#include <mutex>
#include <list>

namespace nltools
{
  namespace threading
  {
    class ContextBoundMessageQueue : public sigc::trackable
    {
     public:
      using tMessage = std::function<void(void)>;
      explicit ContextBoundMessageQueue(Glib::RefPtr<Glib::MainContext> context);
      ~ContextBoundMessageQueue();

      void pushMessage(tMessage&& m);

     private:
      bool doQueuedStuff();

      Glib::RefPtr<Glib::MainContext> m_context;
      std::list<tMessage> m_queue;
      std::recursive_mutex m_mutex;
      GSource* m_source = nullptr;
    };
  }
}
