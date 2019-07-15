#pragma once
#include <functional>
#include <memory>
#include <proxies/hwui/descriptive-layouts/Conditions/ConditionBase.h>

class ConditionRegistry
{
 public:
  typedef ConditionBase* tCondition;
  tCondition getLambda(std::string key);
  static ConditionRegistry& get();

 private:
  ConditionRegistry();
  std::unordered_map<std::string, std::unique_ptr<ConditionBase>> m_theConditionMap;
};
