#include <fstream>
#include <Application.h>
#include <Options.h>
#include <tools/json.h>
#include "LayoutParser.h"
#include "LayoutFactory.h"
#include "ConditionRegistry.h"
#include <boost/algorithm/string.hpp>
#include <regex>
#include <tools/StringTools.h>

using json = nlohmann::json;

namespace DescriptiveLayouts
{
    using tConditionList = std::list<std::function<bool()>>;
  template<class T>
    bool readFieldFromJson(json j, Glib::ustring key, std::function<T(std::string)> converter, std::list<Selector>& outList)
    {
      auto it = j.find(key);
      if(it != j.end())
      {
        T x = converter(*it);
        outList.push_back(x);
        return true;
      }
      return false;
    }

  std::list<Selector> toSelectors(json selector)
  {
    std::list<Selector> selectors;

    if(!readFieldFromJson<UIFocus>(selector, "UIFocus", toUIFocus, selectors))
      selectors.push_back(UIFocus::Any);
    if(!readFieldFromJson<UIMode>(selector, "UIMode", toUIMode, selectors))
      selectors.push_back(UIMode::Any);
    if(!readFieldFromJson<UIDetail>(selector, "UIDetail", toUIDetail, selectors))
      selectors.push_back(UIDetail::Any);
    return selectors;
  }

  Point toPoint(json pt)
  {
    if(pt.is_string())
    {
      std::string compact = pt;
      std::vector<std::string> splits;
      boost::split(splits, compact, boost::is_any_of(","));
      return Point(std::stoi(splits[0]), std::stoi(splits[1]));
    }
    else
    {
      auto x = pt.at("X");
      auto y = pt.at("Y");
      return Point(x, y);
    }
  }


  std::string removeSpaces(std::string str) {
    str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
  }

  std::string removeLastCharacter(std::string str) {
    str.pop_back();
    return str;
  }

  ControlInstance::EventConnections parseEventConnections(json j)
  {
    ControlInstance::EventConnections ret;
    auto it = j.find("Events");
    if(it != j.end())
    {
      auto connections = StringTools::splitStringOnAnyDelimiter(*it, ',');
      for(auto& connection: connections) {

        if(connection.find("=>") == connection.npos)
          DebugLevel::throwException("Event Routing syntax error: missing \"=>\"", connection);

        auto parts = StringTools::splitStringOnStringDelimiter(connection, "=>");
        auto eventTargetParts = StringTools::splitStringOnAnyDelimiter(parts[1], '[');

        auto eventSource = toEventSources(removeSpaces(parts[0]));
        auto eventTargetObject = removeSpaces(eventTargetParts[0]);
        auto eventTargetProperty = toPrimitiveProperty(removeSpaces(removeLastCharacter(eventTargetParts[1])));

        ret.push_back( { eventSource, eventTargetObject, eventTargetProperty });
      }
    }
    return ret;
  }

  LayoutClass::ControlInstanceList toControlInstanceList(json j)
  {
    LayoutClass::ControlInstanceList l;

    for(json::iterator control = j.begin(); control != j.end(); ++control)
    {
      auto controlInstances = control.key();
      auto controlClasses = control.value().at("Class");
      auto point = toPoint(control.value().at("Position"));
      auto eventConnections = parseEventConnections(control.value());
      l.emplace_back(controlInstances, controlClasses, point, eventConnections);
    }
    return l;
  }

  LayoutClass::EventSinkList toEventSinkList(json j)
  {
    LayoutClass::EventSinkList l;
    for(json::iterator eventSink = j.begin(); eventSink != j.end(); ++eventSink)
    {
      l.push_back(EventSinkMapping(toButtons(eventSink.key()), toEventSinks(eventSink.value())));
    }
    return l;
  }

  tConditionList toConditions(json j)
  {
    tConditionList ret;
    for(json::iterator condition = j.begin(); condition != j.end(); ++condition)
    {
      auto conditonStrings = StringTools::splitStringOnAnyDelimiter(condition.value(), ',');
      for(auto conditionString: conditonStrings) {
        ret.push_back(ConditionRegistry::get().getLambda(conditionString));
      }
    }
    return ret;
  }

  void parseLayout(json j)
  {
    for(json::iterator layout = j.begin(); layout != j.end(); ++layout)
    {
      auto name = layout.key();

      DebugLevel::info("importing layout", name);


      auto layoutContent = layout.value();
      auto selectorContent = layoutContent.at("Selector");
      tConditionList selectonConditions;

      auto controlContent = layoutContent.at("Controls");
      auto eventSinkContent = layoutContent.at("EventSinks");

      if(layoutContent.find("Conditions") != layoutContent.end()) {
        auto conditionContent = layoutContent.at("Conditions");
        selectonConditions = toConditions(conditionContent);
      }

      auto id = name;
      auto selectors = toSelectors(selectorContent);
      auto controls = toControlInstanceList(controlContent);
      auto sinkMappings = toEventSinkList(eventSinkContent);
      BoledLayoutFactory::get().registerLayout(id, selectors, controls, sinkMappings, selectonConditions);
    }
  }

  void importLayout(const std::string &fileName)
  {
    DebugLevel::warning("importing layouts from file", fileName);
    std::ifstream i(fileName);
    json j;
    i >> j;

    auto it = j.find("layouts");
    if(it != j.end())
    {
      json layouts = *it;
      parseLayout(layouts);
    }

    BoledLayoutFactory::get().sortByPriority();
  }

}
