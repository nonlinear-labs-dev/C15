#pragma once

#include "playground.h"

class SearchQuery
{
 public:
  enum class Mode
  {
    And,
    Or
  };

  enum class Fields : short
  {
    Name,
    Comment,
    DeviceName
  };

  SearchQuery(const Glib::ustring &query, Mode mode, std::vector<Fields> &&searchFields);
  SearchQuery(const Glib::ustring &query, const Glib::ustring &mode, std::vector<Fields> &&searchFields);

  bool iterate(std::function<bool(const Glib::ustring &, const std::vector<Fields> &fields)> cb) const;
  std::vector<Fields> getFields() const;
  static void registerTests();

 private:
  std::list<Glib::ustring> m_query;
  std::vector<Fields> m_searchFields;
  Mode m_mode;
};
