#pragma once

#include "TemplateEnums.h"
#include <proxies/hwui/HWUI.h>

namespace DescriptiveLayouts
{
  class Styleable;

  template<typename ...tStages>
    struct SubTree;

  struct StyleMap
  {
      Glib::ustring name;
      std::map<StyleKey, int> map;
  };

  inline void merge(StyleMap &target, const StyleMap &s)
  {
    DebugLevel::info("Style", s.name, "matches!");

    for(auto &a : s.map)
    {
      DebugLevel::info("set", toString(a.first), "=", std::to_string(a.second));
      target.map[a.first] = a.second;
    }
  }

  template<typename tLast>
    struct SubTree<tLast> : public std::map<tLast, StyleMap>
    {
        void collectStyle(StyleMap &target, tLast key) const
        {
          tryMerge(target, tLast::Any);
          tryMerge(target, key);
        }

        void tryMerge(StyleMap& target, tLast key) const
        {
          try
          {
            merge(target, this->at(key));
          }
          catch(...)
          {
          }
        }
    };

  template<typename tFirst, typename ...tStages>
    struct SubTree<tFirst, tStages...> : public std::map<tFirst, SubTree<tStages...>>
    {
        void collectStyle(StyleMap &target, tFirst first, tStages ... others) const
        {
          tryMerge(target, tFirst::Any, others...);
          tryMerge(target, first, others...);
        }

        void tryMerge(StyleMap &target, tFirst first, tStages ... others) const
        {
          try
          {
            this->at(first).collectStyle(target, others...);
          }
          catch(...)
          {
          }
        }
    };

  class StyleSheet
  {
    public:
      static StyleSheet &get();

      struct StyleSelector
      {
          template<typename ... tArgs>
            StyleSelector(tArgs ... args)
            {
              (void) std::initializer_list<bool> { (setupSelector(args), false)... };
            }

          void setupSelector(UIFocus v)
          {
            f = v;
          }

          void setupSelector(UIMode v)
          {
            m = v;
          }

          void setupSelector(UIDetail v)
          {
            d = v;
          }

          void setupSelector(LayoutClasses v)
          {
            l = v;
          }

          void setupSelector(ControlClasses v)
          {
            cc = v;
          }

          void setupSelector(ControlInstances v)
          {
            ci = v;
          }

          void setupSelector(PrimitiveClasses v)
          {
            pc = v;
          }

          void setupSelector(PrimitiveTag v)
          {
            pt = v;
          }

          void setupSelector(PrimitiveInstances v)
          {
            pi = v;
          }

          UIFocus f = UIFocus::Any;
          UIMode m = UIMode::Any;
          UIDetail d = UIDetail::Any;
          LayoutClasses l = LayoutClasses::Any;
          ControlClasses cc = ControlClasses::Any;
          ControlInstances ci = ControlInstances::Any;
          PrimitiveClasses pc = PrimitiveClasses::Any;
          PrimitiveTag pt = PrimitiveTag::Any;
          PrimitiveInstances pi = PrimitiveInstances::Any;
      };

      void registerStyle(StyleSelector s, StyleMap style)
      {
        registerStyleFull(s.f, s.m, s.d, s.l, s.cc, s.ci, s.pc, s.pt, s.pi, style);
      }

      void registerStyleFull(UIFocus f, UIMode m, UIDetail d, LayoutClasses l, ControlClasses cc, ControlInstances ci,
                             PrimitiveClasses pc, PrimitiveTag pt, PrimitiveInstances pi, const StyleMap &s);

      void applyStyle(UIFocus f, UIMode m, UIDetail d, LayoutClasses l, ControlClasses cc, ControlInstances ci,
                      PrimitiveClasses pc, PrimitiveTag pt, PrimitiveInstances pi, Styleable *target) const;

      void clear();
    private:
      StyleSheet();
  public:
      void registerStyle(UIFocus f, UIMode m, UIDetail d, LayoutClasses l, ControlClasses cc, ControlInstances ci,
                         PrimitiveClasses pc, PrimitiveTag pt, PrimitiveInstances pi, const StyleMap &s);
  private:
      using Styles = SubTree<UIFocus, UIMode, UIDetail, LayoutClasses, ControlClasses, ControlInstances, PrimitiveClasses, PrimitiveTag, PrimitiveInstances>;
      Styles m_styles;
  };

}
