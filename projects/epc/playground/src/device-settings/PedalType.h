#pragma once

#include <nltools/enums/EnumTools.h>
#include "PedalTypes.h"
#include "NLEnumSetting.h"

class PedalType : public NLEnumSetting<PedalTypes>
{
 private:
  typedef NLEnumSetting<PedalTypes> super;

 public:
  PedalType(UpdateDocumentContributor &settings, uint16_t playcontrollerKey);
  ~PedalType() override;

  Glib::ustring getDisplayString() const override;
  const std::vector<Glib::ustring> &getDisplayStrings() const override;
  bool set(tEnum m) override;
  void incDec(int dir, bool wrap) override;

 private:
  void syncExternals(SendReason reason) const override;
  void load(const Glib::ustring &text, Initiator) override;

  uint16_t m_playcontrollerKey;
};
