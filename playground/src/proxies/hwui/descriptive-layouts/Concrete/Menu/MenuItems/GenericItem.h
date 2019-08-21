#pragma once

#include <Application.h>
#include "BasicItem.h"

class GenericItem : public BasicItem
{
 public:
  template <class T, class tCB>
  GenericItem(const T& caption, tCB cb)
      : BasicItem(caption)
      , m_cb{ cb }
  {
  }

  void doAction() override;

 protected:
  std::function<void(void)> m_cb;
};