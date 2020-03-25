#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/Message.h>
#include <proxies/hwui/panel-unit/boled/setup/USBStickAvailableView.h>
#include <proxies/hwui/panel-unit/boled/setup/UpdateAvailableView.h>
#include "USBChangeListener.h"

USBChangeListener::USBChangeListener()
{
  nltools::msg::receive<nltools::msg::USB::USBStatusMessage>(
      nltools::msg::EndPoint::Playground, [](const nltools::msg::USB::USBStatusMessage& msg) {
        USBStickAvailableView::setUsbAvailable(msg.m_usbAvailable);
        UpdateAvailableView::setUpdateAvailable(msg.m_updateAvailable);
      });
}