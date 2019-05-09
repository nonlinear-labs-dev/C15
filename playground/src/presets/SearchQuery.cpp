#include <presets/SearchQuery.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <testing/TestDriver.h>
#include <xml/XmlReader.h>

TestDriver<SearchQuery> driver;

typedef std::list<Glib::ustring> tStrings;

static SearchQuery::Mode fromString(const Glib::ustring &mode)
{
  auto l = mode.lowercase();

  if(l == "and")
    return SearchQuery::Mode::And;

  return SearchQuery::Mode::Or;
}

tStrings trimAll(const tStrings &a)
{
  tStrings r;

  for(auto &s : a)
  {
    auto c = XmlReader::trim(s);
    if(!c.empty())
      r.push_back(c);
  }

  return r;
}

tStrings splitQuotes(const Glib::ustring &str)
{
  tStrings ret;

  auto quotePos = str.find_first_of('"');

  if(quotePos != Glib::ustring::npos)
  {
    auto nextQuote = str.find_first_of('"', quotePos + 1);

    if(nextQuote != Glib::ustring::npos)
    {
      auto sub = str.substr(0, quotePos);
      boost::algorithm::split(ret, sub, boost::is_any_of(" "));
      ret.push_back(str.substr(quotePos + 1, nextQuote - quotePos - 1));

      auto next = splitQuotes(str.substr(nextQuote + 1));
      ret.insert(ret.end(), next.begin(), next.end());
      return trimAll(ret);
    }
  }

  boost::algorithm::split(ret, str, boost::is_any_of(" "));
  return trimAll(ret);
}

SearchQuery::SearchQuery(const Glib::ustring &query, Mode mode, std::vector<Fields> &&fields)
    : m_mode(mode)
    , m_query(splitQuotes(query))
    , m_searchFields(fields)
{
}

SearchQuery::SearchQuery(const Glib::ustring &query, const Glib::ustring &mode, std::vector<Fields> &&fields)
    : SearchQuery(query, fromString(mode), std::move(fields))
{
}

std::vector<SearchQuery::Fields> SearchQuery::getFields() const
{
  return m_searchFields;
}

bool SearchQuery::iterate(std::function<bool(const ustring &, const std::vector<Fields> &)> cb) const
{
  bool match = false;

  for(auto &s : m_query)
  {
    auto trimmed = XmlReader::trim(s);

    if(!trimmed.empty() && trimmed != "#")
    {
      match = cb(trimmed.lowercase(), getFields());

      if(match && m_mode == Mode::Or)
        return true;

      if(!match && m_mode == Mode::And)
        return false;
    }
  }

  return match;
}

void assertStrings(const Glib::ustring &query, tStrings expected)
{
  auto s = splitQuotes(query);

  if(s != expected)
  {
    std::cout << "got: ";

    for(auto r : s)
      std::cout << r << std::endl;

    std::cout << "expected: ";

    for(auto r : expected)
      std::cout << r << std::endl;

    g_warn_if_reached();
  }
}

void SearchQuery::registerTests()
{
  g_test_add_func("/SearchQuery/quoted", []() {
    assertStrings("", tStrings({}));

    assertStrings("abc \"def\" ghi", tStrings({ "abc", "def", "ghi" }));

    assertStrings("abc \"def ghi", tStrings({ "abc", "\"def", "ghi" }));

    assertStrings("abc    def   ghi", tStrings({ "abc", "def", "ghi" }));

    assertStrings("\"abc    def   ghi\"", tStrings({ "abc    def   ghi" }));

    assertStrings("\"aaa \"\"bbb\"", tStrings({ "aaa", "bbb" }));

    assertStrings("\"aaa \"\"bbb bbb\"", tStrings({ "aaa", "bbb bbb" }));

    assertStrings("\"aaa \"\"bbb ccc", tStrings({ "aaa", "\"bbb", "ccc" }));
  });
}
