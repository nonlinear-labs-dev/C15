#pragma once

#include "tools/Uuid.h"
#include <functional>

class Preset;
class Bank;

class PresetManagerCursor
{
 public:
  using Matcher = std::function<bool(const Preset*)>;

  explicit PresetManagerCursor(Matcher matcher = [](auto) { return true; });

  void moveToSelected();

  bool nextBank();
  [[nodiscard]] bool canNextBank() const;

  bool previousBank();
  [[nodiscard]] bool canPreviousBank() const;

  bool nextPreset();
  [[nodiscard]] bool canNextPreset() const;

  bool previousPreset();
  [[nodiscard]] bool canPreviousPreset() const;

  [[nodiscard]] Preset* getPreset() const;
  [[nodiscard]] Uuid getPresetUuid() const;
  [[nodiscard]] Bank* getBank() const;
  [[nodiscard]] Uuid getBankUuid() const;

  [[nodiscard]] std::string getPresetNumberString() const;
  [[nodiscard]] std::string getPresetName() const;
  [[nodiscard]] std::string getPresetType() const;
  [[nodiscard]] std::string getBankName() const;

 private:
  bool advanceBank(int i);
  bool advancePreset(int i);
  [[nodiscard]] bool canAdvanceBank(int i) const;

  Preset* findFirstMatchingPreset(Bank* bank) const;
  Preset* findLastMatchingPreset(Bank* bank) const;

  Matcher m_matcher;
  Uuid m_preset;
  Uuid m_bank;
};
