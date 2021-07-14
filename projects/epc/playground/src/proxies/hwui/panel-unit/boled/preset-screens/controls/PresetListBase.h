#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/buttons.h>
#include <functional>

class Application;
class PresetListContent;
class PresetListHeader;

class PresetListBase : public ControlWithChildren
{
 private:
  typedef ControlWithChildren super;

 public:
  ~PresetListBase() override;

  bool redraw(FrameBuffer &fb) override;
  bool animateSelectedPreset(std::function<void()> cb);

  void setBankFocus();

  virtual std::pair<size_t, size_t> getSelectedPosition() const = 0;

  virtual bool onButton(Buttons i, bool down, ButtonModifiers modifiers) = 0;
  virtual void onRotary(int inc, ButtonModifiers modifiers) = 0;

 protected:
  PresetListBase(const Rect &pos, bool showBankArrows);
  bool isTransparent() const override;

  PresetListContent *m_content = nullptr;
  PresetListHeader *m_header = nullptr;
};
