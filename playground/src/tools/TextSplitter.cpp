#include "TextSplitter.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "testing/TestDriver.h"

TextSplitter::TextSplitter(shared_ptr<Font> font, double width, const Glib::ustring &text)
{
  list<Glib::ustring> newLined;
  boost::split(newLined, text, boost::is_any_of("\n"));

  for(Glib::ustring line : newLined)
  {
    boost::split(words, line, boost::is_any_of(" "));

    while(!words.empty())
      lines.push_back(eatWords(font, width, words));
  }
}
Glib::ustring TextSplitter::eatWords(shared_ptr<Font> font, double width, list<Glib::ustring> &words)
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

const list<Glib::ustring> &TextSplitter::getLines()
{
  return lines;
}

namespace TEST
{
  struct StringTests
  {
    template <typename... tParts>
    static Glib::ustring formatString(const Glib::ustring &format, const tParts &... parts)
    {
      stringstream str;

      for(Glib::ustring::const_iterator it = format.begin(); it != format.end(); it++)
      {
        if(*it != '%')
        {
          str << *(it);
        }
        else
        {
          Glib::ustring::const_iterator next = it;
          next++;

          if(next != format.end())
          {
            if(*next == '%')
              str << '%';
            else
              formatString_getNthPart(str, ::atoi(&(*next.base())), parts...);

            it = next;
          }
        }
      }

      return str.str();
    }

    static void registerTests()
    {
      g_test_add_func("/strings/concat_utf8", []() {
        Glib::ustring in("\u24b6\u24b7\u24b8\u24b9");
        g_assert(in.length() == 4);
        stringstream str;
        str << in;
        Glib::ustring out = str.str();
        g_assert(out.length() == 4);
      });

      g_test_add_func("/strings/iterate_utf8", []() {
        Glib::ustring in("\u24b6\u24b7\u24b8\u24b9");
        g_assert(in.length() == 4);
        stringstream str;

        for(auto it = in.begin(); it != in.end(); it++)
        {
          gchar txt[5];
          gint numBytes = g_unichar_to_utf8(*it, txt);
          str.write(txt, numBytes);
        }

        Glib::ustring out = str.str();
        g_assert(out.length() == 4);
      });
    }
  };

  static TestDriver<StringTests> tester;
}
