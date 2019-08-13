#pragma once
#include <functional>
#include <memory>
#include <proxies/hwui/descriptive-layouts/Conditions/ConditionBase.h>
#include <tools/Signal.h>
#include <map>

class ConditionRegistry : public sigc::trackable
{
 public:
  typedef ConditionBase* tCondition;
  tCondition getLambda(const std::string& key);
  static ConditionRegistry& get();
  sigc::connection onChange(const std::function<void()>& cb);

 private:
  ConditionRegistry();
  void onConditionChanged();

  Signal<void> m_signal;
  std::map<std::string, std::unique_ptr<ConditionBase>> m_theConditionMap;

  friend class ConditionBase;
};
