#include "Algorithm.h"
#include "Element.h"
#include "Expression.h"

namespace DescriptiveLayouts
{

  enum class ResolveResult
  {
    Done,
    Error,
    Again
  };

  static bool isUnresolvedReference(const Element& e)
  {
    return !e.refersTo.empty() && e.refersTo[0] != '@';
  }

  static bool containsUnresolvedReferences(const Element& e)
  {
    if(isUnresolvedReference(e))
      return true;

    for(auto& c : e.children)
      if(containsUnresolvedReferences(c))
        return true;

    return false;
  }

  template <typename T> static void concat(T& out, const T& in)
  {
    out.insert(out.end(), in.begin(), in.end());
  }

  template <typename E> void mergeIfUnset(E& target, const E& src, const E& undef)
  {
    if(target == undef)
      target = src;
  }

  template <typename E> void copyIfSet(E& target, const E& src, const E& undef)
  {
    if(src != undef)
      target = src;
  }

  template <typename T, int i = std::tuple_size_v<T> - 1> struct StyleProcessor
  {
    static void merge(T& target, const T& source, const T& undef)
    {
      mergeIfUnset(std::get<i>(target), std::get<i>(source), std::get<i>(undef));
      StyleProcessor<T, i - 1>::merge(target, source, undef);
    }

    static void copy(T& target, const T& source, const T& undef)
    {
      copyIfSet(std::get<i>(target), std::get<i>(source), std::get<i>(undef));
      StyleProcessor<T, i - 1>::copy(target, source, undef);
    }
  };

  template <typename T> struct StyleProcessor<T, 0>
  {
    static void merge(T& target, const T& source, const T& undef)
    {
      mergeIfUnset(std::get<0>(target), std::get<0>(source), std::get<0>(undef));
    }

    static void copy(T& target, const T& source, const T& undef)
    {
      copyIfSet(std::get<0>(target), std::get<0>(source), std::get<0>(undef));
    }
  };

  template <typename Out> static void recurseAndProduceFlat(const Element& in, Element collector, Out out)
  {
    collector.name += "/" + in.name;

    if(in.conditions && !collector.conditions)
      collector.conditions = std::unique_ptr<Expression>(in.conditions->clone());
    else if(in.conditions && collector.conditions)
      collector.conditions = std::make_unique<AndExpression>(collector.conditions.release(), in.conditions->clone());

    concat(collector.fires, in.fires);
    concat(collector.inits, in.inits);
    concat(collector.reactions, in.reactions);

    auto movedInPos = in.position.getMovedBy(collector.position.getLeftTop());
    collector.position = collector.position.getIntersection(movedInPos);

    collector.eventProvider = in.eventProvider;
    collector.refersTo = in.refersTo;

    StyleProcessor<Element::Styles>::copy(collector.style, in.style, Element::undefinedStyles);

    if(!in.refersTo.empty())
      out(collector);

    for(auto& c : in.children)
      recurseAndProduceFlat(c, collector, out);
  }

  static void makeFlat(const Element& in, Element& out)
  {
    Element collector(nullptr, "");
    recurseAndProduceFlat(in, collector, [&](const auto& e) { out.children.push_back(e); });
  }

  Element flatten(const Element& tree)
  {
    Element flat;
    makeFlat(tree, flat);
    return flat;
  }

  static bool deepMerge(Element& tgt, const Element& src)
  {
    // TODO check cross references
    // TODO check parent/child references

    for(auto& c : src.children)
      tgt.children.push_back(c);

    if(src.conditions && !tgt.conditions)
      tgt.conditions = std::unique_ptr<Expression>(src.conditions->clone());
    else if(src.conditions && tgt.conditions)
      tgt.conditions = std::make_unique<AndExpression>(tgt.conditions.release(), src.conditions->clone());

    concat(tgt.fires, src.fires);
    concat(tgt.inits, src.inits);
    concat(tgt.reactions, src.reactions);

    mergeIfUnset(tgt.eventProvider, src.eventProvider, EventProviders::None);

    StyleProcessor<Element::Styles>::merge(tgt.style, src.style, Element::undefinedStyles);

    tgt.position = src.position;
    tgt.refersTo.clear();
    return true;
  }

  static ResolveResult resolve(Element& e)
  {
    for(auto& c : e.children)
    {
      if(isUnresolvedReference(c))
      {
        const auto& referencedElement = c.findRelativeElement();

        if(!containsUnresolvedReferences(referencedElement))
        {
          if(!deepMerge(c, referencedElement))
            return ResolveResult::Error;

          return ResolveResult::Again;
        }
      }

      ResolveResult childResult = resolve(c);
      if(childResult != ResolveResult::Done)
        return childResult;
    }
    return ResolveResult::Done;
  }

  bool resolveReferences(Element& e)
  {
    for(;;)
    {
      switch(resolve(e))
      {
        case ResolveResult::Done:
          return true;

        case ResolveResult::Error:
          return false;

        default:
          continue;
      }
    }
    return false;
  }
}
