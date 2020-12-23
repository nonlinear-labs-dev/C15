#pragma once

#include <string>
#include <glibmm/ustring.h>
#include <vector>
#include <tools/Uuid.h>

class Preset;

class PresetUseCases
{
 public:
  explicit PresetUseCases(Preset* preset);
  explicit PresetUseCases(std::vector<Preset*> presets);

  void rename(const std::string& newName);
  void setComment(const Glib::ustring& comment);

  void setAttribute(const Glib::ustring& key, const Glib::ustring& value);

 private:
  std::vector<Preset*> m_presets;
};
