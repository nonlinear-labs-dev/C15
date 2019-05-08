#pragma once

#include "playground.h"
#include "PresetManager.h"

class SearchQuery
{
 public:
  enum class Mode
  {
    And,
    Or
  };

  SearchQuery(const Glib::ustring &query, Mode mode, std::vector<PresetManager::presetInfoSearchFields> &&searchFields);
  SearchQuery(const Glib::ustring &query, const Glib::ustring &mode,
              std::vector<PresetManager::presetInfoSearchFields> &&searchFields);
  virtual ~SearchQuery();
  bool iterate(
      function<bool(const Glib::ustring &, std::vector<PresetManager::presetInfoSearchFields> fields)> cb) const;
  std::vector<PresetManager::presetInfoSearchFields> getFields() const;
  static void registerTests();

 private:
  std::list<Glib::ustring> m_query;
  std::vector<PresetManager::presetInfoSearchFields> m_searchFields;
  Mode m_mode;
};
