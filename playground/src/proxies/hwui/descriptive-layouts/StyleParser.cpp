#include <Application.h>
#include <fstream>
#include "StyleParser.h"
#include "Styles.h"
#include <tuple>
#include <tools/json.h>

using json = nlohmann::json;

namespace DescriptiveLayouts
{
  StyleSheet::StyleSelector parseSelector(json selector)
  {
    StyleSheet::StyleSelector theSelector;

    try
    {
      theSelector.pt = selector.at("Tag");
    }
    catch(...)
    {
    }

    try
    {
      theSelector.f = toUIFocus(selector.at("UIFocus"));
    }
    catch(...)
    {
    }

    try
    {
      theSelector.m = toUIMode(selector.at("UIMode"));
    }
    catch(...)
    {

    }

    try
    {
      theSelector.d = toUIDetail(selector.at("UIDetail"));
    }
    catch(...)
    {
    }

    try
    {
      theSelector.l = selector.at("LayoutClasses");
    }
    catch(...)
    {
    }

    try
    {
      theSelector.cc = selector.at("ControlClasses");
    }
    catch(...)
    {
    }

    try
    {
      theSelector.ci = selector.at("ControlInstances");
    }
    catch(...)
    {
    }

    try
    {
      theSelector.pc = toPrimitiveClasses(selector.at("PrimitiveClasses"));
    }
    catch(...)
    {
    }

    try
    {
      theSelector.pi = selector.at("PrimitiveInstances");
    }
    catch(...)
    {
    }

    return theSelector;
  }

  StyleMap parseStylePairs(json styles)
  {
    StyleMap theStyles{};
    for(json::iterator style = styles.begin(); style != styles.end(); ++style)
    {
      auto styleKey = toStyleKey(style.key());

      switch(styleKey)
      {
        case StyleKey::BorderStyle:
          theStyles.map.emplace(styleKey, (int) StyleValues::toBorderStyle(style.value()));
          break;
        case StyleKey::BackgroundColor:
          theStyles.map.emplace(styleKey, (int) StyleValues::toColor(style.value()));
          break;
        case StyleKey::Color:
          theStyles.map.emplace(styleKey, (int) StyleValues::toColor(style.value()));
          break;
        case StyleKey::FontSize:
          theStyles.map.emplace(styleKey, (int) style.value());
          break;
        case StyleKey::TextAlign:
          theStyles.map.emplace(styleKey, (int) StyleValues::toAlignment(style.value()));
          break;
      }
    }
    return theStyles;
  }

  void registerStyle(std::string name, json style)
  {
    auto selector = parseSelector(style.at("selector"));
    auto styles = parseStylePairs(style.at("styles"));
    styles.name = name;
    StyleSheet::get().registerStyle(selector, styles);
  }

  void importStyles(const std::string &fileName)
  {
    DebugLevel::info("importing styles from file", fileName);
    std::ifstream i(fileName);
    json j;
    i >> j;

    auto it = j.find("styles");
    if(it != j.end())
    {
      json styles = *it;
      for(json::iterator style = styles.begin(); style != styles.end(); ++style)
        registerStyle(style.key(), style.value());
    }
  }

}
