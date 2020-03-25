#include <glibmm/main.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/panel-unit/boled/setup/USBStickAvailableView.h>
#include <sigc++/functors/mem_fun.h>

bool USBStickAvailableView::m_usbAvailable = false;

USBStickAvailableView::USBStickAvailableView()
    : SetupLabel("", Rect(0, 0, 0, 0))
{
  updateLabel();
  Glib::MainContext::get_default()->signal_timeout().connect_seconds(mem_fun(this, &USBStickAvailableView::updateLabel),
                                                                     2);
}

USBStickAvailableView::~USBStickAvailableView() = default;

bool USBStickAvailableView::updateLabel()
{
  if(usbIsReady())
  {
    setText("Yes");
  }
  else
  {
    setText("No");
  }
  return true;
}

bool USBStickAvailableView::usbIsReady()
{
  return m_usbAvailable;
}

void USBStickAvailableView::setUsbAvailable(bool usbAvailable)
{
  m_usbAvailable = usbAvailable;
}
