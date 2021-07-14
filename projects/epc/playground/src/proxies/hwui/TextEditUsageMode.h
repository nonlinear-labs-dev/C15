#pragma once

#include "UsageMode.h"
#include <tuple>
#include <Application.h>
#include <proxies/hwui/TwoStateLED.h>
#include <tools/Signal.h>

class ButtonRepeat;

class TextEditUsageMode : public UsageMode
{
 private:
  typedef UsageMode super;

 public:
  explicit TextEditUsageMode(const Glib::ustring &initalText);
  ~TextEditUsageMode() override;

  void setup() override;
  bool onButtonPressed(Buttons buttonID, ButtonModifiers modifiers, bool state) override;

  Glib::ustring getKeyLabel(Buttons buttonID) const;
  sigc::connection onTextChanged(const sigc::slot<void, const Glib::ustring &> &cb);

  gunichar getCharAtRelativeToCursorPosition(int pos) const;
  bool isInserting() const;
  void moveCursor(int step);
  Glib::ustring getText() const;

  enum class Layout
  {
    Normal,
    Shift,
    Symbol,
    SymbolShift
  };

  Layout getLayout() const;

  void toggleCapsLock();
  void toggleSymbol();
  void handleShiftButton(bool state);

 private:
  void replaceCharWith(gunichar newChar);
  void clampPosition();
  void insertChar(gunichar c);
  void onCharPressed(gunichar c);
  bool handleSpecialChar(gunichar c);
  Glib::ustring stringizeSpecialChar(gunichar c) const;
  void installButtonRepeat(std::function<void()> cb);
  gunichar getCharForButton(Buttons buttonID) const;

  void updateLeds();
  void chooseLayout();

  Glib::ustring m_text;
  int m_position;
  Signal<void, const Glib::ustring &> m_sigTextChanged;

  Layout m_layout;

  bool m_insert = true;

  bool m_capsLock = false;
  bool m_symbolState = false;
  bool m_shiftState = false;

  typedef std::tuple<gunichar, gunichar, gunichar, gunichar> tButtonAssignment;
  std::map<Buttons, tButtonAssignment> m_buttonMap;
  std::unique_ptr<ButtonRepeat> m_buttonRepeat;
};
