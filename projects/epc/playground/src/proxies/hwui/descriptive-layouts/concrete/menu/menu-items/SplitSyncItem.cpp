#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/SplitPointSyncParameters.h>
#include "SplitSyncItem.h"
#include "LeftAligned9pxCaptionLabel.h"
#include <proxies/hwui/descriptive-layouts/concrete/menu/menu-items/controls/CaptionLabel.h>
#include <device-settings/SyncSplitSettingUseCases.h>

void SplitSyncItem::doAction()
{
  auto useCases = SyncSplitSettingUseCases::get();
  useCases.toggleSyncSetting();
}

SplitSyncItem::SplitSyncItem(const Rect& r)
    : BasicItemWithValueText("Link Splitpoints", "", r)
{

  Application::get().getSettings()->getSetting<SplitPointSyncParameters>()->onChange(
      sigc::mem_fun(this, &SplitSyncItem::onSettingChanged));

  m_enterLabel->setText({ "" });
}

void SplitSyncItem::onSettingChanged(const Setting* s)
{
  if(auto split = dynamic_cast<const SplitPointSyncParameters*>(s))
  {
    if(split->get())
    {
      m_valueLabel->setText("Enabled");
    }
    else
    {
      m_valueLabel->setText("Disabled");
    }
  }
}

bool SplitSyncItem::redraw(FrameBuffer& fb)
{
  return BasicItem::redraw(fb) | drawEnterIndication(fb);
}
