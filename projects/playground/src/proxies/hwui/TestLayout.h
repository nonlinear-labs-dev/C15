#pragma once

#include <proxies/hwui/Layout.h>
#include <proxies/hwui/OLEDProxy.h>

class TestLayout : public Layout
{
 private:
  typedef Layout super;

 public:
  explicit TestLayout(OLEDProxy& parent);
  ~TestLayout() override = default;

  void iterate();
  bool redraw(FrameBuffer& fb) override;

 private:
  enum Tests
  {
    Full = 0,
    Gradient = 1,
    Border = 2,
    NumTests
  };

  int m_test = Full;
};