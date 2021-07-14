#pragma once

#include <proxies/hwui/Layout.h>

class DebugLayout : public Layout
{
 protected:
  using super = Layout;

 public:
  explicit DebugLayout(Glib::ustring e);
  bool onButton(Buttons i, bool down, ::ButtonModifiers modifiers) override;
};
