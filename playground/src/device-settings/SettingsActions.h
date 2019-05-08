#pragma once

#include "http/RPCActionManager.h"

class Settings;

class SettingsActions : public RPCActionManager
{
 private:
  typedef RPCActionManager super;

 public:
  SettingsActions(Settings& settings);
  virtual ~SettingsActions();

 private:
  SettingsActions(const SettingsActions& other) = delete;
  SettingsActions& operator=(const SettingsActions&) = delete;
};
