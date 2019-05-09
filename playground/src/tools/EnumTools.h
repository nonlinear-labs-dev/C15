#pragma once

#include "playground.h"
#include <device-settings/DebugLevel.h>
#include <string>
#include <map>
#include <boost/algorithm/string.hpp>

namespace EnumTools
{
  template <typename enum_type, typename base_type, typename struct_type>
  std::map<enum_type, std::string> createMap(const std::string &def)
  {
    std::map<enum_type, std::string> ret;

    auto invalidValue = (base_type) -1;
    auto numEnums = sizeof(struct_type) / sizeof(base_type);
    base_type plain[numEnums];

    for(size_t i = 0; i < numEnums; i++)
      plain[i] = invalidValue;

    new(plain) struct_type;
    base_type currentValue = 0;

    for(size_t i = 0; i < numEnums; i++)
    {
      if(plain[i] == invalidValue)
        plain[i] = currentValue;
      else
        currentValue = plain[i];
      currentValue++;
    }

    std::vector<std::string> strs;
    boost::split(strs, def, boost::is_any_of(","));

    g_assert(strs.size() == numEnums);

    for(size_t idx = 0; idx < numEnums; idx++)
    {
      std::string key = strs[idx];
      key = boost::trim_copy(key);
      auto pos = key.find_first_of(" =,;\n\r");
      if(pos != std::string::npos)
        key = key.substr(0, pos);

      ret[(enum_type) plain[idx]] = key;
    }

    return ret;
  }

  template <typename enum_type> std::map<std::string, enum_type> reverse(const std::map<enum_type, std::string> &m)
  {
    std::map<std::string, enum_type> ret;
    for(auto &a : m)
      ret[a.second] = a.first;

    return ret;
  }
}

#define ENUM(enumName, type, enums...)                                                                                 \
  enum class enumName : type                                                                                           \
  {                                                                                                                    \
    enums                                                                                                              \
  };                                                                                                                   \
                                                                                                                       \
  inline enumName to##enumName(const std::string &e)                                                                   \
  {                                                                                                                    \
    struct __attribute__((packed)) Values                                                                              \
    {                                                                                                                  \
      type enums;                                                                                                      \
    };                                                                                                                 \
    static auto map = EnumTools::reverse(EnumTools::createMap<enumName, type, Values>(#enums));                        \
    auto it = map.find(e);                                                                                             \
    if(it != map.end())                                                                                                \
      return it->second;                                                                                               \
    DebugLevel::throwException("Could not find value", e, "in enum map for", #enumName);                               \
    return (enumName) 0;                                                                                               \
  }                                                                                                                    \
  inline std::string toString(const enumName &e)                                                                       \
  {                                                                                                                    \
    struct __attribute__((packed)) Values                                                                              \
    {                                                                                                                  \
      type enums;                                                                                                      \
    };                                                                                                                 \
    static auto map = EnumTools::createMap<enumName, type, Values>(#enums);                                            \
    auto it = map.find(e);                                                                                             \
    if(it != map.end())                                                                                                \
      return it->second;                                                                                               \
    DebugLevel::throwException("Could not find value", (int) e, "in enum map for", #enumName);                         \
    return "";                                                                                                         \
  }
