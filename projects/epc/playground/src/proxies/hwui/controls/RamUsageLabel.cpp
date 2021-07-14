#include <Application.h>
#include <device-settings/UsedRAM.h>
#include <sigc++/adaptors/hide.h>
#include <device-settings/TotalRAM.h>
#include "RamUsageLabel.h"
#include "device-settings/Settings.h"

RamUsageLabel::RamUsageLabel(const Rect &r)
    : Label(r)
{
  auto settings = Application::get().getSettings();
  settings->getSetting<UsedRAM>()->onChange(sigc::hide<0>(sigc::mem_fun(this, &RamUsageLabel::setDirty)));
}

StringAndSuffix RamUsageLabel::getText() const
{
  auto settings = Application::get().getSettings();
  auto used = settings->getSetting<UsedRAM>();
  auto total = settings->getSetting<TotalRAM>();
  return StringAndSuffix { used->getDisplayString() + "/" + total->getDisplayString() + "MB", 0 };
}
