#pragma once

#include "proxies/hwui/Layout.h"

class Label;
class UsageMode;
class TextEditUsageMode;

class RenameLayout : public Layout
{
 private:
  typedef Layout super;

 public:
  RenameLayout();
  ~RenameLayout() override;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  bool onRotary(int inc, ButtonModifiers modifiers) override;

  void init() override;

 protected:
  virtual void commit(const Glib::ustring &newName) = 0;
  virtual Glib::ustring getInitialText() const = 0;

  virtual void cancel();

  std::shared_ptr<TextEditUsageMode> m_textUsageMode;

 private:
  virtual void onTextChanged(const Glib::ustring &text);
  void replaceUsageMode();
  void addLetters();
  void addControlKeys();
  std::shared_ptr<UsageMode> m_oldUsageMode;
};
