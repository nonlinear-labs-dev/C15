#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/controls/Rect.h>
#include <sigc++/connection.h>

class MonoGroupControl : public ControlWithChildren
{
 public:
  explicit MonoGroupControl(const Rect& r);
  ~MonoGroupControl() override;
  void rebuild();

 private:
  sigc::connection m_connection;
  std::array<std::array<Control*, 2>, 2> m_grid;
};
