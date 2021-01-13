#pragma once
#include <glibmm/ustring.h>
#include <tools/Uuid.h>

class Bank;
class Preset;

class BankUseCases
{
 public:
  explicit BankUseCases(Bank* bank);

  void selectPreset(int pos, bool directLoad);
  void stepPresetSelection(int inc, bool directLoad);

  void renameBank(const Glib::ustring& name);
  void setBankComment(const Glib::ustring& comment);

  void moveBank(const std::string& x, const std::string& y);

  void dropPresets(const std::string& csv);
  void dropBank(const Bank* b);
  void dropBankOnPreset(const Bank* b, const Uuid& presetAnchor);

  void deletePreset(const Preset* p);
  void deletePreset(const Uuid& uuid);

 void setAttribute(const Glib::ustring& key, const Glib::ustring& value);private:
  Bank* m_bank;
};
