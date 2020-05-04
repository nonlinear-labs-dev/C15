#pragma once

#include <proxies/hwui/descriptive-layouts/concrete/menu/SettingLabel.h>
#include "ArrowIncrementDecrementOverlay.h"

template <class tSetting> class ChangeSettingOverlay : public ArrowIncrementDecrementOverlay
{
 public:
  explicit ChangeSettingOverlay(const Rect& r)
      : ArrowIncrementDecrementOverlay(r)
  {
    auto labelWidth = r.getWidth() - 20;
    m_label = addControl(new SettingLabel<tSetting>({ 11, 0, labelWidth, 11 }));
  }

  void onLeft(bool down) override;
  void onRight(bool down) override;
  void onCommit(bool down) override;

  tSetting* getSetting()
  {
    return Application::get().getSettings()->getSetting<tSetting>().get();
  }

  void setLabelHighlight(bool h)
  {
    m_label->setHighlight(h);
  }

 private:
  Control* m_label;
};

template <class tSetting> void ChangeSettingOverlay<tSetting>::onLeft(bool down)
{
  ArrowIncrementDecrementOverlay::onLeft(down);
  if(down)
  {
    getSetting()->incDec(-1, {});
  }
}

template <class tSetting> void ChangeSettingOverlay<tSetting>::onRight(bool down)
{
  ArrowIncrementDecrementOverlay::onRight(down);
  if(down)
  {
    getSetting()->incDec(1, {});
  }
}

template <class tSetting> void ChangeSettingOverlay<tSetting>::onCommit(bool down)
{
  ArrowIncrementDecrementOverlay::onCommit(down);
}
