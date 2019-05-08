#include <Application.h>
#include <device-settings/DeviceName.h>
#include <device-settings/Settings.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <proxies/hwui/panel-unit/boled/setup/RenameDeviceLayout.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>

RenameDeviceLayout::RenameDeviceLayout(shared_ptr<Layout> restore)
    : m_restore(restore)
{
}

RenameDeviceLayout::~RenameDeviceLayout()
{
  auto &boled = Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled();
  boled.reset(m_restore);
}

void RenameDeviceLayout::commit(const Glib::ustring &newName)
{
  Application::get().getSettings()->getSetting<DeviceName>()->set(newName);
}

Glib::ustring RenameDeviceLayout::getInitialText() const
{
  return Application::get().getSettings()->getSetting<DeviceName>()->get();
}
