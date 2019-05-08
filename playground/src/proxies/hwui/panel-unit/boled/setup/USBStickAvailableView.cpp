#include <glibmm/main.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/panel-unit/boled/setup/USBStickAvailableView.h>
#include <sigc++/connection.h>
#include <sigc++/functors/mem_fun.h>
#include <tools/SpawnCommandLine.h>
#include <string>

USBStickAvailableView::USBStickAvailableView()
    : SetupLabel("", Rect(0, 0, 0, 0))
{
  updateLabel();
  Glib::MainContext::get_default()->signal_timeout().connect_seconds(mem_fun(this, &USBStickAvailableView::updateLabel),
                                                                     2);
}

USBStickAvailableView::~USBStickAvailableView()
{
}

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
#ifdef _DEVELOPMENT_PC
  return true;
#endif
  SpawnCommandLine cmd("lsblk");
  return cmd.getStdOutput().find("/mnt/usb-stick") != std::string::npos;
}
