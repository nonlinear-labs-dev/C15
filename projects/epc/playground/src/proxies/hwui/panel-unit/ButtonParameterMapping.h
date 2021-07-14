#pragma once

#include "playground.h"
#include <proxies/hwui/buttons.h>
#include <map>
#include <list>

class ButtonParameterMapping
{
 public:
  ButtonParameterMapping();
  virtual ~ButtonParameterMapping();

  void forEachButton(std::function<void(Buttons, const std::list<int> &)> cb);
  [[nodiscard]] Buttons findButton(int param) const;
  [[nodiscard]] std::list<int> findParameters(Buttons button) const;

 private:
  void addMapping(Buttons buttonID, std::initializer_list<int> parameterIDs);

  std::map<int, Buttons> m_paramIDToButtonID;
  std::map<Buttons, std::list<int>> m_buttonIDToParam;
};
