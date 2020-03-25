#pragma once

#include <glibmm/ustring.h>
#include <memory>
#include "Font.h"

class Control;

class TextCropper
{
 public:
  static Glib::ustring shortenStringIfNeccessary(std::shared_ptr<Font> font, const Glib::ustring &text,
                                                 double maxWidth);

 protected:
  static Glib::ustring preserveEndShorten(const std::shared_ptr<Font> &font, const Glib::ustring &text,
                                          double maxWidth);
  static Glib::ustring rigidSplit(const std::shared_ptr<Font> &font, const Glib::ustring &text, double maxWidth);
  static Glib::ustring edgeCases(int min, Glib::ustring::size_type len, const Glib::ustring &ustring);

  static Glib::ustring spaceBasedCropping(const Glib::ustring &text, const std::shared_ptr<Font> &font,
                                          double maxWidth);
};
