#include <proxies/hwui/panel-unit/boled/setup/UpdateAvailableView.h>

static const auto c_updateFileName = "/mnt/usb-stick/nonlinear-c15-update.tar";

UpdateAvailableView::UpdateAvailableView()
    : super("", Rect(0, 0, 0, 0))
{
  poll();
  Glib::MainContext::get_default()->signal_timeout().connect_seconds(mem_fun(this, &UpdateAvailableView::poll), 5);
}

UpdateAvailableView::~UpdateAvailableView()
{
}

bool UpdateAvailableView::poll()
{
  if(updateExists())
  {
    setText("Yes");
  }
  else
  {
    setText("No");
  }
  return true;
}

bool UpdateAvailableView::updateExists()
{
  return g_file_test(c_updateFileName, GFileTest::G_FILE_TEST_EXISTS);
}
