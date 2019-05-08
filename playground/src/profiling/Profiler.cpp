#include "Profiler.h"

#ifdef _PROFILING

#include <mutex>
#include <atomic>
#include <dlfcn.h>

#include <iomanip>

#include <pthread.h>
#include <stdio.h>
#include <glib.h>
#include <map>
#include <list>
#include <algorithm>
#include <iostream>
#include <execinfo.h>
#include <glib.h>
#include <gio/gio.h>

static thread_local Profiler::TLS tls;

bool Profiler::m_isEnabled = false;

std::string resolveSymbol(void *addr)
{
  if(addr == nullptr)
    return "<null>";

  char **res = backtrace_symbols(&addr, 1);
  std::string ret(res[0]);
  free(res);

  std::string pattern("/playground");
  auto pos = ret.rfind(pattern);
  if(pos != std::string::npos)
  {
    ret = ret.substr(pos + pattern.length());
  }

  return ret;
}

struct ProfileNode
{
  typedef std::map<void *, ProfileNode *> tChildren;

  ProfileNode(ProfileNode *parent, void *symbol)
      : m_parent(parent)
      , m_timeSeen(0)
      , m_symbol(symbol)
  {
  }

  int accumulateChildren()
  {
    for(auto &c : m_children)
    {
      m_timeSeen += c.second->accumulateChildren();
    }

    return m_timeSeen;
  }

  void fillMap(std::map<void *, uint64_t> &hotspots)
  {
    for(auto &c : m_children)
    {
      hotspots[c.first] += c.second->m_timeSeen;
      c.second->fillMap(hotspots);
    }
  }

  void printChildrenSorted(float threshold, int indent, int threadSeen)
  {
    std::cout << std::setprecision(2);

    std::list<tChildren::value_type> sorted;

    for(auto &c : m_children)
    {
      auto ratio = 100 * c.second->m_timeSeen / (float) m_timeSeen;

      if(ratio >= threshold || indent < 2)
      {
        sorted.push_back(c);
      }
    }

    sorted.sort([](const tChildren::value_type &a, const tChildren::value_type &b) -> bool {
      return a.second->m_timeSeen > b.second->m_timeSeen;
    });

    for(auto &s : sorted)
    {
      auto ratioOfParent = 100 * s.second->m_timeSeen / (float) m_timeSeen;
      auto ratioOfall = 100 * s.second->m_timeSeen / (float) threadSeen;

      uint64_t childrenSeen = 0;
      for(auto &c : s.second->m_children)
      {
        childrenSeen += c.second->m_timeSeen;
      }

      auto self = 100 * (s.second->m_timeSeen - childrenSeen) / (float) threadSeen;
      auto indentation = std::string(indent, ' ');

      std::cout << indentation;
      std::cout << "self-overall:" << self << "; overall: " << ratioOfall << ";from parent: " << ratioOfParent << ": ";
      std::cout << resolveSymbol(s.first) << std::endl;
      s.second->printChildrenSorted(threshold, indent + 1, threadSeen);
    }
  }

  ProfileNode *m_parent = nullptr;
  tChildren m_children;
  std::atomic<uint64_t> m_timeSeen = {};
  void *m_symbol;
};

Profiler &Profiler::get()
{
  static Profiler profiler;
  return profiler;
}

Profiler::TLS::TLS()
{
  m_bg = std::thread([=]() {
    while(!m_close)
    {
      if(auto p = stackTip)
      {
        p->m_timeSeen++;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  });
}

void Profiler::TLS::stop()
{
  m_close = true;

  if(m_bg.joinable())
    m_bg.join();
}

Profiler::TLS::~TLS()
{
  stop();
}

Profiler::Profiler()
{
}

Profiler::~Profiler()
{
}

void Profiler::enable(bool v)
{
  m_isEnabled = v;
}

void Profiler::onEnterFunction(void *this_fn)
{
  if(!m_isEnabled)
    return;

  if(tls.stackTip == nullptr)
  {
    static std::mutex m;
    m.lock();
    m_tls.push_back(&tls);
    tls.stackTip = tls.root = new ProfileNode(nullptr, nullptr);
    m.unlock();
  }

  auto it = tls.stackTip->m_children.find(this_fn);

  if(it != tls.stackTip->m_children.end())
  {
    tls.stackTip = it->second;
  }
  else
  {
    ProfileNode *newTip = new ProfileNode(tls.stackTip, this_fn);
    tls.stackTip->m_children[this_fn] = newTip;
    tls.stackTip = newTip;
  }

  tls.stackTip->m_timeSeen++;
}

void Profiler::onLeaveFunction(void *this_fn)
{
  if(!m_isEnabled)
    return;

  if(tls.stackTip && tls.stackTip->m_parent)
    tls.stackTip = tls.stackTip->m_parent;
}

void Profiler::print()
{
  Profiler::m_isEnabled = false;

  std::cout << std::endl << "[BEGIN] Profiler:" << std::endl;

  for(auto tls : m_tls)
  {
    tls->stop();
    print(tls->root);
    printHotspots(tls->root);
  }

  std::cout << std::endl << "[END] Profiler:" << std::endl << std::endl;
}

void Profiler::printAllCallstacks()
{
  std::cout << std::endl << "[BEGIN] Call stacks:" << std::endl;

  for(auto tls : m_tls)
  {
    std::cout << std::endl << "[BEGIN] Call stack:" << std::endl;

    auto tip = tls->stackTip;

    while(tip)
    {
      std::cout << resolveSymbol(tip->m_symbol) << std::endl;
      tip = tip->m_parent;
    }

    std::cout << std::endl << "[END] Call stack" << std::endl << std::endl;
  }

  std::cout << std::endl << "[END] Call stacks" << std::endl << std::endl;
}

void Profiler::print(ProfileNode *stack)
{
  std::cout << "[BEGIN] Printing stack" << std::endl;
  stack->accumulateChildren();
  stack->printChildrenSorted(15, 0, stack->m_timeSeen);
  std::cout << "[END] Printing stack" << std::endl << std::endl;
}

void Profiler::printHotspots(ProfileNode *stack)
{
  std::map<void *, uint64_t> hotspots;
  stack->fillMap(hotspots);
  std::cout << "[BEGIN] Printing calls" << std::endl;

  std::list<std::map<void *, int>::value_type> sorted;

  for(auto c : hotspots)
  {
    if(c.second > 100)
      sorted.push_back(c);
  }

  sorted.sort([](const std::map<void *, int>::value_type &a, const std::map<void *, int>::value_type &b) {
    return a.second > b.second;
  });

  for(auto it : sorted)
  {
    std::cout << it.second << ": " << resolveSymbol(it.first) << std::endl;
  }

  std::cout << "[END] Printing calls" << std::endl << std::endl;
}

extern "C"
{
  void __cyg_profile_func_enter(void *this_fn, void *call_site)
  {
    if(Profiler::m_isEnabled)
      Profiler::get().onEnterFunction(this_fn);
  }

  void __cyg_profile_func_exit(void *this_fn, void *call_site)
  {
    if(Profiler::m_isEnabled)
      Profiler::get().onLeaveFunction(this_fn);
  }
}
#endif
