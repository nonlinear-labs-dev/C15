#include "Dumper.h"
#include "Element.h"
#include <nltools/logging/Log.h>

namespace DescriptiveLayouts
{
  void Dumper::dump(const Element& e, size_t indent)
  {
    std::string i(indent, ' ');
    nltools::Log::notify(i, "Element: ", e.name);
    nltools::Log::notify(i, " pos: ", e.position.getX(), e.position.getY());
    nltools::Log::notify(i, " size: ", e.position.getWidth(), e.position.getHeight());
    nltools::Log::notify(i, " ref: ", e.refersTo);
    nltools::Log::notify(i, " event provider: ", toString(e.eventProvider));

    if(e.conditions)
    {
#warning "log conditions"
    }

    if(!e.fires.empty())
    {
      nltools::Log::notify(i, " fires: ", std::to_string(e.fires.size()));
      for(auto c : e.fires)
        nltools::Log::notify(i, "   Fires: ", c);
    }

    if(!e.inits.empty())
    {
      nltools::Log::notify(i, " inits: ", std::to_string(e.inits.size()));
      for(auto c : e.inits)
        nltools::Log::notify(i, "   Initializes: ", c);
    }

    if(!e.reactions.empty())
    {
      nltools::Log::notify(i, " reacts: ", std::to_string(e.reactions.size()));
      for(auto c : e.reactions)
        nltools::Log::notify(i, "   Reacts to: ", c);
    }

    if(!e.children.empty())
    {
      nltools::Log::notify(i, " children: ", std::to_string(e.children.size()));
      for(auto& c : e.children)
        dump(c, indent + 4);
    }
  }
}
