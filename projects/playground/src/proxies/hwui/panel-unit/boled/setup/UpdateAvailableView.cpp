#include <proxies/hwui/panel-unit/boled/setup/UpdateAvailableView.h>
#include <glibmm/main.h>

bool UpdateAvailableView::m_updateAvailable = false;

UpdateAvailableView::UpdateAvailableView()
    : super("", Rect(0, 0, 0, 0))
{
  poll();
  Glib::MainContext::get_default()->signal_timeout().connect_seconds(mem_fun(this, &UpdateAvailableView::poll), 5);
}

UpdateAvailableView::~UpdateAvailableView() = default;

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
  return m_updateAvailable;
}

void UpdateAvailableView::setUpdateAvailable(bool updateAvailable)
{
  m_updateAvailable = updateAvailable;
}
