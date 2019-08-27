#pragma once

#include <proxies/hwui/HWUIEnums.h>
#include <functional>
#include <tools/variant.hpp>

namespace DescriptiveLayouts
{
  class Selector
  {
    using Criteria = nonstd::variant<UIFocus, UIMode, UIDetail>;

    struct Tester
    {
      FocusAndMode fam;

      Tester(FocusAndMode fam)
          : fam(fam)
      {
      }

      bool operator()(const UIFocus f) const
      {
        return f == fam.focus || f == UIFocus::Any || fam.focus == UIFocus::Any;
      }

      bool operator()(const UIMode m) const
      {
        return m == fam.mode || m == UIMode::Any || fam.mode == UIMode::Any;
      }

      bool operator()(const UIDetail d) const
      {
        return d == fam.detail || d == UIDetail::Any || fam.detail == UIDetail::Any;
      }

      bool operator()(UIFocus f)
      {
        return f == fam.focus || f == UIFocus::Any || fam.focus == UIFocus::Any;
      }

      bool operator()(UIMode m)
      {
        return m == fam.mode || m == UIMode::Any || fam.mode == UIMode::Any;
      }

      bool operator()(UIDetail d)
      {
        return d == fam.detail || d == UIDetail::Any || fam.detail == UIDetail::Any;
      }
    };

   public:
    Selector(Criteria c)
        : criteria(c)
    {
    }

    Selector(UIFocus f)
        : criteria(f)
    {
    }

    Selector(UIMode m)
        : criteria(m)
    {
    }

    Selector(UIDetail d)
        : criteria(d)
    {
    }

    bool operator==(const Selector &other) const
    {
      return criteria == other.criteria;
    }

    bool test(FocusAndMode fam) const
    {
      auto tester = Tester(fam);
      auto c = criteria;
      return nonstd::visit(tester, c);
    }

    UIFocus getFocus() const {
      return nonstd::get<UIFocus >(criteria);
    }

    UIMode getMode() const {
      return nonstd::get<UIMode >(criteria);
    }

    UIDetail getDetail() const {
      return nonstd::get<UIDetail >(criteria);
    }

   private:
    Criteria criteria;

    friend class ConsistencyChecker;
  };
}
