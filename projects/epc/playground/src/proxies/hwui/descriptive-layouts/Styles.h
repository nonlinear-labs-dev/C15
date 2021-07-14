#pragma once

#include "TemplateEnums.h"
#include <proxies/hwui/HWUIEnums.h>

namespace DescriptiveLayouts
{
  class Styleable;

  template <typename... tStages> struct SubTree;

  struct StyleMap
  {
    Glib::ustring name;
    std::map<StyleKey, int> map;
  };

  namespace Detail
  {
    struct StyleSelector
    {
      template <typename... tArgs> explicit StyleSelector(tArgs... args)
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

    using IterationCallback = std::function<void(const Detail::StyleSelector &, const StyleMap &)>;
  }

  inline void merge(StyleMap &target, const StyleMap &s)
  {
    for(auto &a : s.map)
      target.map[a.first] = a.second;
  }

  template <typename tLast> struct SubTree<tLast> : public std::map<tLast, StyleMap>
  {
    void collectStyle(StyleMap &target, tLast key) const
    {
      tryMerge(target, tLast::Any);
      tryMerge(target, key);
    }

    void tryMerge(StyleMap &target, tLast key) const
    {
      auto it = this->find(key);
      if(it != this->end())
        merge(target, it->second);
    }

    void iterateStyles(const Detail::StyleSelector &sel, Detail::IterationCallback cb) const
    {
      for(const auto &a : *this)
      {
        Detail::StyleSelector cp = sel;
        if(a.first != tLast::Any)
        {
          cp.setupSelector(a.first);
          cb(cp, a.second);
        }
      }
    }
  };

  template <typename tFirst, typename... tStages>
  struct SubTree<tFirst, tStages...> : public std::map<tFirst, SubTree<tStages...>>
  {
    void collectStyle(StyleMap &target, tFirst first, tStages... others) const
    {
      tryMerge(target, tFirst::Any, others...);
      tryMerge(target, first, others...);
    }

    void collectInplace(StyleMap &target, tFirst first, tStages... others) const
    {
      tryMerge(target, tFirst::Any, others...);
      tryMerge(target, first, others...);
    }

    void tryMerge(StyleMap &target, tFirst first, tStages... others) const
    {
      auto it = this->find(first);
      if(it != this->end())
        it->second.collectStyle(target, others...);
    }

    void iterateStyles(const Detail::StyleSelector &sel, Detail::IterationCallback cb) const
    {
      for(const auto &a : *this)
      {
        Detail::StyleSelector cp = sel;
        if(a.first != tFirst::Any)
        {
          cp.setupSelector(a.first);
          a.second.iterateStyles(cp, cb);
        }
      }
    }
  };

  class StyleSheet
  {
   public:
    static StyleSheet &get();

    void registerStyle(Detail::StyleSelector s, const StyleMap &style)
    {
      registerStyleFull(s.f, s.m, s.d, s.l, s.cc, s.ci, s.pc, s.pt, s.pi, style);
    }

    void registerStyleFull(UIFocus f, UIMode m, UIDetail d, LayoutClasses l, ControlClasses cc, ControlInstances ci,
                           PrimitiveClasses pc, PrimitiveTag pt, PrimitiveInstances pi, const StyleMap &s);

    void applyStyle(UIFocus f, UIMode m, UIDetail d, LayoutClasses l, ControlClasses cc, ControlInstances ci,
                    PrimitiveClasses pc, PrimitiveTag pt, PrimitiveInstances pi, Styleable *target) const;

    void iterateStyles(Detail::IterationCallback cb) const
    {
      Detail::StyleSelector sel;
      m_styles.iterateStyles(sel, std::move(cb));
    }

    void clear();

   private:
    StyleSheet();

   private:
    using Styles = SubTree<UIFocus, UIMode, UIDetail, LayoutClasses, ControlClasses, ControlInstances, PrimitiveClasses,
                           PrimitiveTag, PrimitiveInstances>;

    Styles m_styles;

    friend class ConsistencyChecker;
  };
}
