#include <Application.h>
#include <fstream>
#include <tools/StringTools.h>
#include "StyleParser.h"
#include "Styles.h"
#include "ControlRegistry.h"
#include "ControlParser.h"
#include <tuple>
#include <nltools/nlohmann/json.hpp>
#include <device-settings/DebugLevel.h>
#include <boost/algorithm/string.hpp>
#include <tools/ExceptionTools.h>
#include "JsonHelpers.h"

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
      if(splits.size() == 4)
        return Rect(std::stoi(splits[0]), std::stoi(splits[1]), std::stoi(splits[2]), std::stoi(splits[3]));
      else
        return Rect(0, 0, 0, 0);
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

  Rect parseRect(json value, const std::string& key)
  {
    try
    {
      return parseRect(value.at("Rect"));
    }
    catch(...)
    {
      DebugLevel::throwException("No rect defined for primitive", key);
      return { 0, 0, 0, 0 };
    }
  }

  std::list<PrimitiveInstance> createPrimitives(const json& primitives)
  {
    std::list<PrimitiveInstance> lP;
    JSONTools::forEachJsonChild(primitives, [&lP](const auto& obj) {
      auto value = obj.value();
      auto key = obj.key();
      auto primClass = JSONTools::getFromJson<PrimitiveClasses>(value, "Class", to<PrimitiveClasses>);
      auto tag = JSONTools::getFromJson<PrimitiveTag>(value, "Tag");
      auto rect = parseRect(value, key);
      lP.emplace_back(key, primClass, rect, tag);
    });
    return lP;
  }

  std::list<ControlClass> createControls(const json& controlText)
  {
    std::list<ControlClass> l_cc;
    JSONTools::forEachJsonChild(controlText, [&l_cc](const auto& obj) {
      l_cc.emplace_back(ControlClass(obj.key(), createPrimitives(obj.value())));
    });
    return l_cc;
  }

  void registerControls(const json& j)
  {
    for(auto& c : createControls(j))
    {
      ControlRegistry::get().registerControl(std::move(c));
    }
  }

  void importControls(const std::string& fileName)
  {
    DebugLevel::gassy("importing controls from file", fileName);
    std::ifstream i(fileName);
    json j;
    i >> j;

    JSONTools::forEachJsonInTag(j, "controls", [&](const auto& item) { registerControls(item); });
  }
}
