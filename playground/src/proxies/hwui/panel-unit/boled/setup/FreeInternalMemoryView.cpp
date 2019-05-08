#include <Application.h>
#include <device-info/DeviceInformation.h>
#include <device-info/FreeDiscSpaceInformation.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/panel-unit/boled/setup/FreeInternalMemoryView.h>

FreeInternalMemoryView::FreeInternalMemoryView()
    : base_type("", Rect(0, 0, 0, 0))
{
  Application::get().getDeviceInformation()->getItem<FreeDiscSpaceInformation>()->onChange(
      mem_fun(this, &FreeInternalMemoryView::onSettingChanged));
}

FreeInternalMemoryView::~FreeInternalMemoryView()
{
}

void FreeInternalMemoryView::onSettingChanged(const DeviceInformationItem *s)
{
  auto v = dynamic_cast<const FreeDiscSpaceInformation *>(s);
  setText(v->get());
}
