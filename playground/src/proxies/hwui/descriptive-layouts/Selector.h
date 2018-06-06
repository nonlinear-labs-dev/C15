#pragma once

#include <proxies/hwui/HWUIEnums.h>
#include <functional>
#include <variant>

namespace DescriptiveLayouts
{
  class Selector
  {
      using Criteria = std::variant<UIFocus, UIMode, UIDetail>;

      struct Tester
      {
          FocusAndMode fam;

          Tester(FocusAndMode fam) :
              fam(fam)
          {
          }
          bool operator()(UIFocus f)
          {
            return f == fam.focus || f == UIFocus::Any || fam.focus == UIFocus::Any;
          }

          bool operator()(UIMode m)
          {
            return m == fam.mode || m == UIMode::Any || fam.mode == UIMode::Any;
          }

          bool operator ()(UIDetail d)
          {
            return d == fam.detail || d == UIDetail::Any || fam.detail == UIDetail::Any;
          }
      };

    public:
      Selector(Criteria c) :
          criteria(c)
      {
      }

      Selector(UIFocus f) :
        criteria(f) {}

      Selector(UIMode m) :
        criteria(m) {}

      Selector(UIDetail d) :
        criteria(d) {}

      bool test(FocusAndMode fam) const
      {
        return std::visit(Tester(fam), criteria);
      }

    private:
      Criteria criteria;
  };
}
