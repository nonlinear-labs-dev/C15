#pragma once

#include <nltools/enums/EnumTools.h>

ENUM(WifiSettings, int, Disabled, Enabled, Querying)

#include "NLEnumSetting.h"

class WifiSetting : public NLEnumSetting<WifiSettings>, public sigc::trackable
{
 public:
  typedef NLEnumSetting<WifiSettings> super;
  explicit WifiSetting(UpdateDocumentContributor& settings);

  bool set(tEnum m) override;
  bool persistent() const override;

 private:
  bool pollAccessPointRunning();

  void onPollReturned(GPid pid, int result);
  void onCommandReturned(GPid pid, int exitStatus);

  sigc::connection m_pollConnection;
};
