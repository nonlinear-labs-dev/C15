#include <Application.h>
#include <device-info/DeviceInformation.h>
#include <device-info/SoftwareVersion.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/panel-unit/boled/setup/DeviceInfoItemView.h>

DeviceInfoItemView::DeviceInfoItemView(DeviceInformationItem* item, Duration updateInterval)
    : base_type("", Rect(0, 0, 0, 0))
{
  item->onChange(mem_fun(this, &DeviceInfoItemView::onInfoChanged));

  if(updateInterval != Duration::zero())
  {
    m_update.setCallback([=]() {
      onInfoChanged(item);
      m_update.refresh(updateInterval);
    });
    m_update.refresh(updateInterval);
  }
}

DeviceInfoItemView::~DeviceInfoItemView()
{
}

void DeviceInfoItemView::onInfoChanged(const DeviceInformationItem* s)
{
  setText(s->getDisplayString());
}
