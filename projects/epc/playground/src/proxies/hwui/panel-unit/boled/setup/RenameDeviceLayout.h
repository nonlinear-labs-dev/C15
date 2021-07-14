#pragma once

#include <proxies/hwui/panel-unit/boled/preset-screens/RenameLayout.h>

class RenameDeviceLayout : public RenameLayout
{
 public:
  explicit RenameDeviceLayout(std::shared_ptr<Layout> restore);
  ~RenameDeviceLayout() override;

  void commit(const Glib::ustring &newName) override;
  Glib::ustring getInitialText() const override;

 private:
  std::shared_ptr<Layout> m_restore;
};
