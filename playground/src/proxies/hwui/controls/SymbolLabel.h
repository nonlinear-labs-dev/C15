#pragma once

#include "Label.h"

class SymbolLabel : public Label
{
 private:
  typedef Label super;

 public:
  // public methods
  SymbolLabel(const Glib::ustring& text, const Rect& pos);
  virtual ~SymbolLabel();

 protected:
  virtual shared_ptr<Font> getFont() const override;
  virtual int getFontHeight() const override;

 private:
  // private methods
  SymbolLabel(const SymbolLabel& other);
  SymbolLabel& operator=(const SymbolLabel&);
};
