#pragma once
#include <proxies/hwui/Layout.h>
#include <sigc++/connection.h>

class Label;

class BOLEDScreenSaver : public Layout
{
 public:
  explicit BOLEDScreenSaver(OLEDProxy& oled);
  ~BOLEDScreenSaver() override;
  void init() override;
  bool onRotary(int inc, ButtonModifiers modifiers) override;

 private:
  void destroy();
  bool animate();
  sigc::connection m_animation;
  sigc::connection m_editBufferConnection;

  std::pair<int, int> m_vel = { -1, -1 };
  Label* m_label = nullptr;
};
