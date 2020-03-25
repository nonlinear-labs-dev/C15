#include <presets/SearchQuery.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <xml/XmlReader.h>
#include <iostream>

static SearchQuery::Mode fromString(const Glib::ustring &mode)
{
  auto l = mode.lowercase();

  if(l == "and")
    return SearchQuery::Mode::And;

  return SearchQuery::Mode::Or;
}

std::list<Glib::ustring> trimAll(const std::list<Glib::ustring> &a)
{
  std::list<Glib::ustring> r;

  for(auto &s : a)
  {
    auto c = XmlReader::trim(s);
    if(!c.empty())
      r.push_back(c);
  }

  return r;
}

std::list<Glib::ustring> SearchQuery::splitQuotes(const Glib::ustring &str)
{
  std::list<Glib::ustring> ret;

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

bool SearchQuery::iterate(const std::function<bool(const Glib::ustring &, const std::vector<Fields> &)> &cb) const
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