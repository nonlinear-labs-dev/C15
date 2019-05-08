#pragma once

#include <tuple>
#include <memory>

namespace UNDO
{
  template <typename... T> class SwapData : private std::tuple<T...>
  {
    typedef std::tuple<T...> tBase;
    typedef SwapData<T...> tThis;

   public:
    typedef std::shared_ptr<tThis> tPtr;

    SwapData(T... args)
        : tBase(args...)
    {
    }

    virtual ~SwapData()
    {
    }

    template <int N> typename std::tuple_element<N, tBase>::type &get()
    {
      return std::get<N>(*this);
    }

    template <int N = 0, typename tSwap> void swapWith(tSwap &other)
    {
      swap(get<N>(), other);
    }
  };

  template <typename... T> static typename SwapData<T...>::tPtr createSwapData(T... data)
  {
    return typename SwapData<T...>::tPtr(new SwapData<T...>(data...));
  }

} /* namespace UNDO */
