#pragma once

#include "playground.h"
#include <proxies/hwui/Font.h>
#include <list>

class TextSplitter
{
 private:
  list<Glib::ustring> words;
  list<Glib::ustring> lines;

 public:
  TextSplitter(shared_ptr<Font> font, double width, const Glib::ustring &text);

  const list<Glib::ustring> &getLines();
  Glib::ustring eatWords(shared_ptr<Font> font, double width, list<Glib::ustring> &words);
};
