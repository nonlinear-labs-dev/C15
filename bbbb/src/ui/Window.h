#pragma once

#ifdef _DEVELOPMENT_PC

#include <gtkmm-3.0/gtkmm.h>
#include "Boled.h"
#include "PanelUnit.h"
#include "PlayPanel.h"
#include "Ribbon.h"
#include "LowerRibbon.h"
#include "Keyboard.h"

namespace nltools
{
  namespace msg
  {
    struct SetPanelLEDMessage;
    struct SetOLEDMessage;
  }
}

class Window : public Gtk::Window
{
 public:
  Window();
  virtual ~Window();

 private:
  void onFrameBufferMessageReceived(const nltools::msg::SetOLEDMessage &msg);
  void onFrameBufferTimestampedMessageReceived(const nltools::msg::SetTimestampedOledMessage &msg);

  void onPanelLEDsMessageReceived(const nltools::msg::SetPanelLEDMessage &msg);

  Gtk::VBox m_box;
  Gtk::VBox m_ribbonBox;
  PlayPanel m_playPanel;
  PanelUnit m_editPanel;
  Ribbon m_ribbonUp;
  LowerRibbon m_ribbonDown;
  Keyboard m_keyboard;
};

#endif
