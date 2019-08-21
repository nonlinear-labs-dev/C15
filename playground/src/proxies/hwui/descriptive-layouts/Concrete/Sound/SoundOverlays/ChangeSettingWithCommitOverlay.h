#pragma once

#include "ChangeSettingOverlay.h"

template <class tSetting> class ChangeSettingWithCommitOverlay : public ChangeSettingOverlay<tSetting>
{
 protected:
  using ChangeSettingOverlay<tSetting>::m_label;

 public:
  template <class tCB>
  ChangeSettingWithCommitOverlay(const Rect& r, tCB cb)
      : ChangeSettingOverlay<tSetting>(r)
      , m_commitCB{ cb }
  {
  }

  void onCommit(bool down) override
  {
    ChangeSettingOverlay<tSetting>::onCommit(down);
    if(down)
    {
      m_commitCB();
    }

    m_label->setHighlight(down);
  }

 protected:
  std::function<void(void)> m_commitCB;
};
