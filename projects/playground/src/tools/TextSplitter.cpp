#include "TextSplitter.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <proxies/hwui/Font.h>

TextSplitter::TextSplitter(std::shared_ptr<Font> font, double width, const Glib::ustring &text)
{
  std::list<Glib::ustring> newLined;
  boost::split(newLined, text, boost::is_any_of("\n"));

  for(Glib::ustring line : newLined)
  {
    boost::split(words, line, boost::is_any_of(" "));

    while(!words.empty())
      lines.push_back(eatWords(font, width, words));
  }
}
Glib::ustring TextSplitter::eatWords(std::shared_ptr<Font> font, double width, std::list<Glib::ustring> &words)
{
  Glib::ustring lineText = "";
  int spaceWidth = font->getStringWidth(" ");

  while(!words.empty())
  {
    Glib::ustring word = words.front();
    words.pop_front();

    int w = 0;

    w = font->getStringWidth(word);

    if(w < width)
    {
      if(!lineText.empty())
        lineText += " ";

      lineText += word;
      width -= w;
      width -= spaceWidth;
    }
    else
    {
      if(lineText.empty())
      {
        int half = word.length() / 2;

        if(half == 0)
        {
          if(!lineText.empty())
            lineText += " ";

          lineText += word;
          width -= w;
          width -= spaceWidth;
        }
        else
        {
          Glib::ustring first = word.substr(0, half);
          Glib::ustring second = word.substr(half);
          words.push_front(second);
          words.push_front(first);
        }
      }
      else
      {
        words.push_front(word);
        break;
      }
    }
  }

  return lineText;
}

const std::list<Glib::ustring> &TextSplitter::getLines()
{
  return lines;
}
