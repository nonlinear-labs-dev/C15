#pragma once

#ifdef _PROFILING

#include <list>
#include <map>
#include <thread>

struct ProfileNode;

class Profiler
{
 public:
  static Profiler &get();
  virtual ~Profiler();

  void onEnterFunction(void *this_fn);
  void onLeaveFunction(void *this_fn);

  void printAllCallstacks();

  void enable(bool v);
  void print();

  struct TLS
  {
    TLS();
    ~TLS();
    void stop();
    ProfileNode *stackTip = nullptr;
    ProfileNode *root = nullptr;
    bool m_close = false;
    std::thread m_bg;
  };

  static bool m_isEnabled;

 private:
  Profiler();
  Profiler(const Profiler &other);
  Profiler &operator=(const Profiler &);

  void print(ProfileNode *stackTip);
  void printHotspots(ProfileNode *stack);

  std::list<TLS *> m_tls;
};

#endif
