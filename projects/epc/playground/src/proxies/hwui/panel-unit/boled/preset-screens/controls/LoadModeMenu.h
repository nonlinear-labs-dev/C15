#pragma once

#include <proxies/hwui/controls/ButtonMenu.h>
#include <nltools/Types.h>
#include <sigc++/connection.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/ShortVsLongPress.h>

class LoadModeMenu : public ControlWithChildren
{
 public:
  explicit LoadModeMenu(const Rect& rect);
  ~LoadModeMenu() override;
  bool redraw(FrameBuffer& fb) override;
  bool onButton(Buttons button, bool down, ButtonModifiers modifiers);

 private:
  void bruteForce();
  void installSingle();
  void installDual();

  sigc::connection m_soundTypeConnection;
  sigc::connection m_directLoadSettingConnection;
  sigc::connection m_loadToPartConnection;

  std::unique_ptr<ShortVsLongPress> m_buttonDHandler;

  static bool isDirectLoadEnabled();
  static bool isLoadToPartEnabled();
  static SoundType getSoundType();

  const std::function<void(void)> m_toggleDirectLoadAction;

 protected:
  void setBackgroundColor(FrameBuffer& fb) const override;
};
