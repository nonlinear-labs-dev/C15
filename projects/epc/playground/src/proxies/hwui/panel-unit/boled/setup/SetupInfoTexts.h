#pragma once

#include <device-settings/TransitionTime.h>
#include <device-settings/TuneReference.h>
#include <device-settings/SyncVoiceGroupsAcrossUIS.h>
ENUM(SetupInfoEntries, int, SettingItem, VelocityCurve, AftertouchCurve, BenderCurve, PedalSetting, WiFiSetting,
     PresetGlitchSuppression, EditSmoothingTime, Pedals, DeviceSettings, DeviceName, SSID, Passphrase, UpdateAvailable,
     FreeMemory, SoftwareVersion, DateTime, WebsiteAddress, SystemInfo, About, EncoderAcceleration,
     RibbonRelativeFactor, SignalFlowIndicator, ScreenSaverTimeout, HardwareUI, USBAvailable, SaveAllBanks,
     RestoreAllBanks, Backup, Setup, TransitionTime, TuneReference, RandomizePart, RandomizeSound, InitPart, InitSound,
     ConvertToSingle, ConvertToSplit, ConvertToLayer, PartLabel, StoreInitSound, ResetInitSound, SyncVoiceGroups);

template <typename tSetting> SetupInfoEntries SettingToSetupInfoEntry()
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

static std::map<SetupInfoEntries, const char*> SetupInfoHeaders {
  { SetupInfoEntries::VelocityCurve, "Velocity Curve" },
  { SetupInfoEntries::SettingItem, "Setting" },
  { SetupInfoEntries::AftertouchCurve, "Aftertouch Curve" },
  { SetupInfoEntries::BenderCurve, "Pitchbender Curve" },
  { SetupInfoEntries::PedalSetting, "Pedal Setting" },
  { SetupInfoEntries::WiFiSetting, "Enable / Disbale WiFi" },
  { SetupInfoEntries::PresetGlitchSuppression, "Preset Glitch Suppression" },
  { SetupInfoEntries::EditSmoothingTime, "Edit Smoothing Time" },
  { SetupInfoEntries::Pedals, "Settings for the 4 Pedals" },
  { SetupInfoEntries::DeviceSettings, "Device Settings" },
  { SetupInfoEntries::SSID, "WiFi-SSID" },
  { SetupInfoEntries::Passphrase, "WiFi-Passphrase" },
  { SetupInfoEntries::UpdateAvailable, "Update Available" },
  { SetupInfoEntries::FreeMemory, "Free Internal Memory" },
  { SetupInfoEntries::SoftwareVersion, "Software Version" },
  { SetupInfoEntries::DateTime, "Date/Time Adjustments" },
  { SetupInfoEntries::WebsiteAddress, "WebUI Address" },
  { SetupInfoEntries::SystemInfo, "System Info" },
  { SetupInfoEntries::About, "About" },
  { SetupInfoEntries::EncoderAcceleration, "Encoder Acceleration" },
  { SetupInfoEntries::RibbonRelativeFactor, "Ribbon Relative Factor" },
  { SetupInfoEntries::SignalFlowIndicator, "Signal Flow Indication" },
  { SetupInfoEntries::HardwareUI, "Hardware UI" },
  { SetupInfoEntries::USBAvailable, "USB Available" },
  { SetupInfoEntries::SaveAllBanks, "Save All Banks to USB" },
  { SetupInfoEntries::RestoreAllBanks, "Restore All Banks from USB" },
  { SetupInfoEntries::Backup, "Backup" },
  { SetupInfoEntries::Setup, "not used.." },
  { SetupInfoEntries::TransitionTime, "Transition Time" },
  { SetupInfoEntries::TuneReference, "Tune Reference" },
  { SetupInfoEntries::RandomizePart, "Randomize Part" },
  { SetupInfoEntries::RandomizeSound, "Randomize Sound" },
  { SetupInfoEntries::InitPart, "Init Part" },
  { SetupInfoEntries::InitSound, "Init Sound" },
  { SetupInfoEntries::ConvertToSingle, "Convert Sound To Single" },
  { SetupInfoEntries::ConvertToSplit, "Convert Sound To Split" },
  { SetupInfoEntries::ConvertToLayer, "Convert Sound To Layer" },
  { SetupInfoEntries::PartLabel, "Dual Sound Part Name" },
  { SetupInfoEntries::StoreInitSound, "Store Init Sound" },
  { SetupInfoEntries::ResetInitSound, "Reset Init Sound" },
  { SetupInfoEntries::ScreenSaverTimeout, "Screen Saver Timeout" },
  { SetupInfoEntries::SyncVoiceGroups, "Sync Parts across UI's" },
  { SetupInfoEntries::DeviceName, "Device Name" }
};

static std::map<SetupInfoEntries, const char*> SetupInfoContent {
  { SetupInfoEntries::VelocityCurve,
    "Velocity Curve lorem ipsum lorem ipsum lorem ipsum lorem ipsum lorem ipsum lorem ipsum lorem ipsumlorem ipsum "
    "lorem ipsum lorem ipsum lorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::SettingItem,
    "Setting lorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::AftertouchCurve,
    "Aftertouch Curve lorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::BenderCurve,
    "Pitchbender Curvelorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::PedalSetting, "Pedal Settinglorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::WiFiSetting, "Enable or Disable internal WiFi - Accesspoint" },
  { SetupInfoEntries::PresetGlitchSuppression,
    "Preset Glitch Suppressionlorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::EditSmoothingTime,
    "Edit Smoothing Timelorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::Pedals,
    "Settings for the 4 Pedalslorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::DeviceSettings,
    "Device Settings submenulorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::SSID, "WiFi SSID Settinglorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::Passphrase, "WiFi Passphrase Settinglorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::UpdateAvailable, "Update Availablelorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::FreeMemory,
    "How much space do we have?lorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::SoftwareVersion,
    "What C15 Software Version?lorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::DateTime, "Adjust Date / Time lorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::WebsiteAddress, "192.168.8.2 lorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::SystemInfo, "System Status / Info submenu lorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::About, "About nl lorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::EncoderAcceleration, "encoder go brrt lorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::RibbonRelativeFactor,
    "ribbon go brrt lorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::SignalFlowIndicator,
    "Signal Flow Indication lorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::HardwareUI,
    "Hardware UI Setting submenu lorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::USBAvailable, "USB Available for backup? lorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::SaveAllBanks, "Save All Banks to USB lorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::RestoreAllBanks,
    "Restore All Banks from USB lorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::Backup, "Backup lorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::Setup, "not used.. lorem ipsumlorem ipsumlorem ipsumlorem ipsumlorem ipsum" },
  { SetupInfoEntries::TransitionTime, "Transition Time is:  loreeeem loreeeem ipsumipsumloreeeem ipsumloreeeem ipsum" },
  { SetupInfoEntries::TuneReference, "Tune Refrerence is : loreeeem loreeeem ipsumipsumloreeeem ipsumloreeeem ipsum" },
  { SetupInfoEntries::RandomizePart, "RandomizePart lorem lorem ipsum libsum checksum" },
  { SetupInfoEntries::RandomizeSound, "Randomize Sound lorem lorem ipsum libsum checksum" },
  { SetupInfoEntries::InitPart, "Init Part lorem lorem ipsum libsum checksum" },
  { SetupInfoEntries::InitSound, "Init Sound lorem lorem ipsum libsum checksum" },
  { SetupInfoEntries::ConvertToSingle,
    "Convert Sound To Single lorem lorem loremlorem loremlorem loremlorem loremlorem loremlorem" },
  { SetupInfoEntries::ConvertToSplit,
    "Convert Sound To Split lorem loremlorem loremlorem loremlorem loremlorem loremlorem loremlorem lorem" },
  { SetupInfoEntries::ConvertToLayer,
    "Convert Sound To Layer lorem loremlorem loremlorem loremlorem loremlorem loremlorem loremlorem loremlorem "
    "lorem" },
  { SetupInfoEntries::PartLabel, "Part Label of Dual Sound Part part?!" },
  { SetupInfoEntries::StoreInitSound, "Store Init Sound TEXT" },
  { SetupInfoEntries::ResetInitSound, "Reset Init Sound TEXT" },
  { SetupInfoEntries::ScreenSaverTimeout,
    "Time after last HWUI interaction / editbuffer change til screen starts saving" },
  { SetupInfoEntries::SyncVoiceGroups, "Sync Parts across UI's TEXT" },
  { SetupInfoEntries::DeviceName, "Device Name Text" }
};