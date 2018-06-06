#include <Application.h>
#include <fstream>
#include <tools/StringTools.h>
#include "StyleParser.h"
#include "Styles.h"
#include "ControlRegistry.h"
#include "ControlParser.h"
#include <tuple>
#include <tools/json.h>
#include <device-settings/DebugLevel.h>
#include <boost/algorithm/string.hpp>
#include <tools/ExceptionTools.h>

using json = nlohmann::json;

namespace DescriptiveLayouts
{
  Rect parseRect(json rect)
  {
    if(rect.is_string())
    {
      std::string compact = rect;
      std::vector<std::string> splits;
      boost::split(splits, compact, boost::is_any_of(","));
      return Rect(std::stoi(splits[0]), std::stoi(splits[1]), std::stoi(splits[2]), std::stoi(splits[3]));
    }
    else
    {
      auto x = rect.at("X");
      auto y = rect.at("Y");
      auto w = rect.at("W");
      auto h = rect.at("H");
      return Rect(x, y, w, h);
    }
  }

  Rect parseRect(json value, const std::string &key)
  {
    try
    {
      return parseRect(value.at("Rect"));
    }
    catch(...)
    {
      DebugLevel::throwException("No rect defined for primitive", key);
      return { 0, 0, 0, 0};
    }
  }

  template<class T>
  constexpr T getFromJson(json j, std::string key)
  {
      auto itProp = j.find(key);
      if(itProp != j.end())
      {
        return *itProp;
      }
      return T { };
  }

  template<class T>
  T getFromJson(json j, std::string key, std::function<T(std::string)> converter)
  {
      auto itProp = j.find(key);
      if(itProp != j.end())
      {
        return converter(*itProp);
      }
      return T { };
  }

  std::list<PrimitiveInstance> createPrimitives(json primitives)
  {
    std::list<PrimitiveInstance> lP;
    for(json::iterator primitive = primitives.begin(); primitive != primitives.end(); ++primitive)
    {
      auto key = primitive.key();
      auto value = primitive.value();

      auto primClass = getFromJson<PrimitiveClasses>(value, "Class", toPrimitiveClasses);
      auto prop = getFromJson<PrimitiveProperty>(value, "Property", toPrimitiveProperty);
      auto tag = getFromJson<PrimitiveTag>(value, "Tag");
      auto defaultText = getFromJson<DefaultText>(value, "Default");
      auto rect = parseRect(value, key);
      lP.emplace_back(key, primClass, rect, tag, defaultText);
    }
    return lP;
  }

  std::list<ControlClass> createControls(json controlText)
  {
    std::list<ControlClass> l_cc;

    for(json::iterator critera = controlText.begin(); critera != controlText.end(); ++critera)
    {
      l_cc.push_back(ControlClass(critera.key(), createPrimitives(critera.value())));
    }

    return l_cc;
  }

  void registerControls(json j)
  {
    for(auto& c : createControls(j))
    {
      ControlRegistry::get().registerControl(std::move(c));
    }
  }

  void importControls(const std::string &fileName)
  {
    DebugLevel::info("importing controls from file", fileName);
    std::ifstream i(fileName);
    json j;
    i >> j;

    auto it = j.find("controls");
    if(it != j.end())
    {
      json control = *it;
      registerControls(control);
    }
  }

}
