#pragma once

#include <glibmm.h>

namespace DETAIL
{
  template <typename T> struct Deleter
  {
    void operator()(T* p)
    {
      g_free(p);
    }
  };

  template <> struct Deleter<GError>
  {
    void operator()(GError* p)
    {
      g_error_free(p);
    }
  };
}

template <typename T> class ScopeFree
{
  T* ptr;

 public:
  ScopeFree(T* p = NULL)
      : ptr(p)
  {
  }

  ~ScopeFree()
  {
    DETAIL::Deleter<T> d;
    d(ptr);
  }

  T** getPtrPtr()
  {
    return &ptr;
  }

  T* get()
  {
    return ptr;
  }

  const T* get() const
  {
    return ptr;
  }

  operator const T*() const
  {
    return ptr;
  }

  operator T*()
  {
    return ptr;
  }

  T* operator->()
  {
    return this->ptr;
  }

  const T* operator->() const
  {
    return this->ptr;
  }

  void reset(T* newPtr = NULL)
  {
    if(newPtr != ptr)
    {
      DETAIL::Deleter<T> d;
      d(ptr);
      ptr = newPtr;
    }
  }

  ScopeFree<T>& operator=(const ScopeFree<T>& other) = delete;
};
