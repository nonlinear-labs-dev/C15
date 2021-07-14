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

    template <typename... Q>
    explicit SwapData(Q &&... args)
        : tBase(std::forward<Q>(args)...)
    {
    }

    virtual ~SwapData() = default;

    template <int N> typename std::tuple_element<N, tBase>::type &get()
    {
      return std::get<N>(*this);
    }

    template <int N = 0, typename tSwap> void swapWith(tSwap &other)
    {
      std::swap(get<N>(), other);
    }
  };

  namespace detail
  {
    template <class X> struct unwrap_refwrapper
    {
      using type = X;
    };

    template <class X> struct unwrap_refwrapper<std::reference_wrapper<X>>
    {
      using type = X &;
    };

    template <class X> using special_decay_t = typename unwrap_refwrapper<typename std::decay<X>::type>::type;
  }

  template <typename... T> static typename SwapData<detail::special_decay_t<T>...>::tPtr createSwapData(T &&... data)
  {
    using S = SwapData<detail::special_decay_t<T>...>;
    return std::make_shared<S>(std::forward<T>(data)...);
  }

} /* namespace UNDO */
