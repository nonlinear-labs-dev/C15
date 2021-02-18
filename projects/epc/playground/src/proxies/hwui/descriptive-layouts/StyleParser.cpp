#include <Application.h>
#include <fstream>
#include "StyleParser.h"
#include "Styles.h"
#include <tuple>
#include <nltools/nlohmann/json.hpp>
#include <device-settings/DebugLevel.h>

using json = nlohmann::json;

namespace DescriptiveLayouts
{
  template <typename T, typename C> void assignIfExists(T &target, const C &coll, const std::string &key)
  {
    auto it = coll.find(key);
    if(it != coll.end())
      target = *it;
  }

  template <typename T, typename C, typename Conv>
  void assignIfExists(T &target, const C &coll, const Conv &conv, const std::string &key)
  {
    auto it = coll.find(key);
    if(it != coll.end())
      target = conv(*it);
  }

  Detail::StyleSelector parseSelector(json selector)
  {
    Detail::StyleSelector theSelector;
    assignIfExists(theSelector.pt, selector, "Tag");
    assignIfExists(theSelector.f, selector, ::to<UIFocus>, "UIFocus");
    assignIfExists(theSelector.m, selector, ::to<UIMode>, "UIMode");
    assignIfExists(theSelector.d, selector, ::to<UIDetail>, "UIDetail");
    assignIfExists(theSelector.l, selector, "LayoutClasses");
    assignIfExists(theSelector.cc, selector, "ControlClasses");
    assignIfExists(theSelector.ci, selector, "ControlInstances");
    assignIfExists(theSelector.pc, selector, to<PrimitiveClasses>, "PrimitiveClasses");
    assignIfExists(theSelector.pi, selector, "PrimitiveInstances");
    return theSelector;
  }

  StyleMap parseStylePairs(json styles)
  {
    StyleMap theStyles {};
    for(json::iterator style = styles.begin(); style != styles.end(); ++style)
    {
      auto styleKey = to<StyleKey>(style.key());

      switch(styleKey)
      {
        case StyleKey::BorderStyle:
          theStyles.map.emplace(styleKey, (int) StyleValues::to<StyleValues::BorderStyle>(style.value()));
          break;
        case StyleKey::SuffixColor:
        case StyleKey::BorderColor:
        case StyleKey::BackgroundColor:
        case StyleKey::HighlightColor:
        case StyleKey::HighlightBackgroundColor:
        case StyleKey::Color:
          theStyles.map.emplace(styleKey, (int) StyleValues::to<StyleValues::Color>(style.value()));
          break;
        case StyleKey::FontSize:
          theStyles.map.emplace(styleKey, (int) style.value());
          break;
        case StyleKey::TextAlign:
          theStyles.map.emplace(styleKey, (int) StyleValues::to<StyleValues::Alignment>(style.value()));
          break;
        case StyleKey::FontDecoration:
          theStyles.map.emplace(styleKey, (int) StyleValues::to<StyleValues::Font>(style.value()));
          break;
      }
    }
    return theStyles;
  }

  void registerStyle(const std::string &name, json style)
  {
    auto selector = parseSelector(style.at("selector"));
    auto styles = parseStylePairs(style.at("styles"));
    styles.name = name;
    StyleSheet::get().registerStyle(selector, styles);
  }

  void importStyles(const std::string &fileName)
  {
    DebugLevel::gassy("importing styles from file", fileName);
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
