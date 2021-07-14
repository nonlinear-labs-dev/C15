#pragma once

#include <glibmm/ustring.h>
#include <nltools/nlohmann/json.hpp>

namespace JSONTools
{
  template <class T> T getFromJson(const nlohmann::json& j, std::string key)
  {
    auto itProp = j.find(key);
    if(itProp != j.cend())
    {
      auto value = j.at(key).get<std::string>();
      return static_cast<T>(value);
    }
    return T {};
  }

  template <class T> T getFromJson(const nlohmann::json& j, std::string key, std::function<T(std::string)> converter)
  {
    auto itProp = j.find(key);
    if(itProp != j.cend())
    {
      return converter(itProp.value());
    }
    return T {};
  }

  template <class T, class tList>
  bool readFieldsFromJson(const nlohmann::json& j, const Glib::ustring& key, std::function<T(std::string)> converter,
                          tList& outList)
  {
    auto it = j.find(key);
    if(it != j.cend())
    {
      T x = converter(*it);
      outList.push_back(x);
      return true;
    }
    return false;
  }

  template <class CB> void forEachJsonChild(const nlohmann::json& j, CB cb)
  {
    for(auto obj = j.cbegin(); obj != j.cend(); ++obj)
    {
      cb(obj);
    }
  }

  template <class CB> void forEachJsonArrayElementChild(const nlohmann::json& j, CB cb)
  {
    for(const auto& entry : j)
    {
      cb(entry);
    }
  }

  template <class CB, typename tTag> void forEachJsonInTag(const nlohmann::json& j, tTag tag, CB cb)
  {
    auto it = j.find(tag);
    if(it != j.cend())
    {
      const nlohmann::json& item = *it;
      cb(item);
    }
  }

  template <typename tTag> const nlohmann::json* findTag(const nlohmann::json& j, tTag tag)
  {
    auto it = j.find(tag);
    if(it != j.cend())
    {
      return &*it;
    }

    return nullptr;
  }
}
