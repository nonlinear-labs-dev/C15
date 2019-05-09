#pragma once

#include "proxies/hwui/DFBLayout.h"

class Label;
class UsageMode;
class TextEditUsageMode;

class RenameLayout : public DFBLayout
{
 private:
  typedef DFBLayout super;

 public:
  RenameLayout();
  virtual ~RenameLayout();

  virtual bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  virtual bool onRotary(int inc, ButtonModifiers modifiers) override;

  virtual void init() override;

 protected:
  virtual void commit(const Glib::ustring &newName) = 0;
  virtual Glib::ustring getInitialText() const = 0;

  virtual void cancel();

  std::shared_ptr<TextEditUsageMode> m_textUsageMode;

 private:
  virtual void onTextChanged(const ustring &text);
  void replaceUsageMode();
  void addLetters();
  void addControlKeys();
  std::shared_ptr<UsageMode> m_oldUsageMode;
};
