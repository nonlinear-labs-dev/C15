#pragma once

#include "BasicItem.h"

class ActionItem : public BasicItem {
public:
  template<class tCap, class tCB>
  ActionItem(tCap caption, tCB cb) : BasicItem(caption), m_cb{cb} {
  }
  void doAction() override;

protected:
  std::function<void()> m_cb;
};
