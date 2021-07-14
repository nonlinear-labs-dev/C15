#pragma once

#include "ChangeSettingOverlay.h"

template <class tSetting> class ChangeSettingWithCommitOverlay : public ChangeSettingOverlay<tSetting>
{
  using super = ChangeSettingOverlay<tSetting>;

 public:
  template <class tCB>
  ChangeSettingWithCommitOverlay(const Rect& r, tCB cb)
      : super(r)
      , m_commitCB { cb }
  {
  }

  void onCommit(bool down) override
  {
    super::onCommit(down);

    if(down)
      m_commitCB();

    super::setLabelHighlight(down);
  }

 private:
  std::function<void(void)> m_commitCB;
};
