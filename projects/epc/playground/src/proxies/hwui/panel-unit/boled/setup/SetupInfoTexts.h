#pragma once

#include <device-settings/TransitionTime.h>
#include <device-settings/TuneReference.h>
#include <device-settings/SyncVoiceGroupsAcrossUIS.h>
#include <CompileTimeOptions.h>
#include <tools/json.h>
#include <fstream>

ENUM(SetupInfoEntries, int, SettingItem, VelocityCurve, AftertouchCurve, BenderCurve, PedalSetting, WiFiSetting,
     PresetGlitchSuppression, EditSmoothingTime, Pedals, DeviceSettings, DeviceName, SSID, Passphrase, UpdateAvailable,
     FreeMemory, SoftwareVersion, DateTime, WebsiteAddress, SystemInfo, About, EncoderAcceleration,
     RibbonRelativeFactor, SignalFlowIndicator, ScreenSaverTimeout, HardwareUI, USBAvailable, SaveAllBanks,
     RestoreAllBanks, Backup, Setup, TransitionTime, TuneReference, RandomizePart, RandomizeSound, InitPart, InitSound,
     ConvertToSingle, ConvertToSplit, ConvertToLayer, PartLabel, StoreInitSound, ResetInitSound, SyncVoiceGroups);

class SetupInfoTexts
{
 public:
  template <typename tSetting> static SetupInfoEntries SettingToSetupInfoEntry()
  {
    if constexpr(std::is_same_v<tSetting, TransitionTime>)
    {
      return SetupInfoEntries::TransitionTime;
    }
    else if constexpr(std::is_same_v<tSetting, TuneReference>)
    {
      return SetupInfoEntries::TuneReference;
    }
    else if constexpr(std::is_same_v<tSetting, SyncVoiceGroupsAcrossUIS>)
    {
      return SetupInfoEntries::SyncVoiceGroups;
    }
  }

  static std::map<SetupInfoEntries, std::string> m_headers;
  static std::map<SetupInfoEntries, std::string> m_content;

  static void loadInfos();

  static std::string getInfoHeader(SetupInfoEntries e);

  static std::string getInfoContent(SetupInfoEntries e);
};
