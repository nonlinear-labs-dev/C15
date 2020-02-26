#include "Element.h"
#include "Expression.h"
#include <boost/algorithm/string.hpp>

namespace DescriptiveLayouts
{

  Element::Element(Element* parent, const std::string& n)
      : parent(parent)
      , name(n)
  {
  }

  Element::Element(const Element& other)
      : parent(other.parent)
      , name(other.name)
      , refersTo(other.refersTo)
      , position(other.position)
      , eventProvider(other.eventProvider)
      , fires(other.fires)
      , reactions(other.reactions)
      , inits(other.inits)
      , conditions(other.conditions ? other.conditions->clone() : nullptr)
      , children(other.children)
      , style(other.style)
  {
  }

  Element::~Element()
  {
  }

  std::ostream& operator<<(std::ostream& out, const Element::Strings& str)
  {
    out << "(";
    for(const auto& a : str)
      out << a << " ";
    out << ")";

    return out;
  }

  void Element::dump(std::ostream& str)
  {
    str << name << " refers to " << refersTo << " at pos " << position.getX() << "," << position.getY() << ","
        << position.getWidth() << "," << position.getHeight() << " reacts to events from " << toString(eventProvider)
        << " fires " << fires << " reacts to " << reactions << " initializes " << inits;

    if(conditions)
    {
      str << " if ";
      conditions->dump(str);
    }

    for(auto& c : children)
    {
      str << std::endl;
      c.dump(str);
    }
  }

  const Element& Element::findChild(const Strings& path) const
  {
    if(path.empty())
      return *this;

    for(auto& c : children)
      if(c.name == path.front())
        return c.findChild(Strings(std::next(path.begin()), path.end()));

    throw std::runtime_error("Reference not found");
  }

  const Element& Element::findReference(const Strings& path) const
  {
    if(name == path.front())
      return findChild(Strings(std::next(path.begin()), path.end()));

    for(auto& c : children)
      if(c.name == path.front())
        return c.findChild(Strings(std::next(path.begin()), path.end()));

    if(!parent)
      throw std::runtime_error("Reference not found");

    return parent->findReference(path);
  }

  const Element& Element::findRelativeElement() const
  {
    Strings path;
    boost::split(path, refersTo, boost::is_any_of("/"));
    return findReference(path);
  }
}
