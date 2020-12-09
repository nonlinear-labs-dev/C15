#include <Application.h>
#include <device-settings/DirectLoadSetting.h>
#include <device-settings/BaseUnitUIMode.h>
#include <device-settings/DebugLevel.h>
#include <device-settings/DeviceName.h>
#include <device-settings/NoteShift.h>
#include <device-settings/ParameterEditModeRibbonBehaviour.h>
#include <device-settings/PresetDragDropEnabled.h>
#include <device-settings/PresetStoreModeSetting.h>
#include <device-settings/SendPresetAsPlaycontrollerWriteFallback.h>
#include <device-settings/Settings.h>
#include <device-settings/VelocityCurve.h>
#include <device-settings/TransitionTime.h>
#include <device-settings/RandomizeAmount.h>
#include <device-settings/RibbonRelativeFactor.h>
#include <device-settings/EncoderAcceleration.h>
#include <device-settings/PedalType.h>
#include <device-settings/AftertouchCurve.h>
#include <device-settings/BenderCurve.h>
#include <device-settings/EditSmoothingTime.h>
#include <device-settings/SSID.h>
#include <device-settings/Passphrase.h>
#include <device-settings/PresetGlitchSuppression.h>
#include <device-settings/DateTimeAdjustment.h>
#include <device-settings/SignalFlowIndicationSetting.h>
#include <device-settings/KioskModeSetting.h>
#include <device-settings/BlockingMainThreadIndication.h>
#include <device-settings/HighlightChangedParametersSetting.h>
#include <http/NetworkRequest.h>
#include <http/UpdateDocumentMaster.h>
#include <Options.h>
#include <serialization/SettingsSerializer.h>
#include <xml/Attribute.h>
#include <xml/FileInStream.h>
#include <xml/FileOutStream.h>
#include <xml/Writer.h>
#include <xml/XmlReader.h>
#include <xml/XmlWriter.h>
#include <xml/VersionAttribute.h>
#include <proxies/playcontroller/PlaycontrollerProxy.h>
#include "WifiSetting.h"
#include "SettingsActions.h"
#include "CrashOnError.h"
#include "LayoutMode.h"
#include "TuneReference.h"
#include "TotalRAM.h"
#include "UsedRAM.h"
#include "SyncVoiceGroupsAcrossUIS.h"
#include "SplitPointSyncParameters.h"
#include "ExternalMidiEnabledSetting.h"
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameter_declarations.h>
#include <parameters/RibbonParameter.h>
#include <device-settings/ScreenSaverTimeoutSetting.h>
#include <iostream>

Settings::Settings(UpdateDocumentMaster *master)
    : super(master)
    , m_actions(std::make_unique<SettingsActions>(*this))
    , m_saveJob(5000, std::bind(&Settings::save, this))
{
  addSetting("DirectLoad", new DirectLoadSetting(*this));
  addSetting("SendPresetAsLPCWriteFallback", new SendPresetAsPlaycontrollerWriteFallback(*this));
  addSetting("PresetStoreModeSetting", new PresetStoreModeSetting(*this));
  addSetting("BaseUnitUIMode", new BaseUnitUIMode(*this));
  addSetting("NoteShift", new NoteShift(*this));
  addSetting("ParameterEditModeRibbonBehaviour", new ParameterEditModeRibbonBehaviour(*this));
  addSetting("DebugLevel", new DebugLevel(*this));
  addSetting("VelocityCurve", new VelocityCurve(*this));
  addSetting("DeviceName", new DeviceName(*this));
  addSetting("PresetDragDropEnabled", new PresetDragDropEnabled(*this));
  addSetting("TransitionTime", new TransitionTime(*this));
  addSetting("RandomizeAmount", new RandomizeAmount(*this));
  addSetting("RibbonRelFactor", new RibbonRelativeFactor(*this));
  addSetting("Pedal1Type", new PedalType(*this, 1));
  addSetting("Pedal2Type", new PedalType(*this, 2));
  addSetting("Pedal3Type", new PedalType(*this, 3));
  addSetting("Pedal4Type", new PedalType(*this, 4));
  addSetting("EncoderAcceleration", new EncoderAcceleration(*this));
  addSetting("LayoutVersionMode", new LayoutMode(this));
  addSetting("AftertouchCurve", new AftertouchCurve(*this));
  addSetting("BenderCurve", new BenderCurve(*this));
  addSetting("EditSmoothingTime", new EditSmoothingTime(*this));
  addSetting("SSID", new SSID(*this));
  addSetting("Passphrase", new Passphrase(*this));
  addSetting("PresetGlitchSuppression", new PresetGlitchSuppression(*this));
  addSetting("DateTimeAdjustment", new DateTimeAdjustment(*this));
  addSetting("SignalFlowIndication", new SignalFlowIndicationSetting(*this));
  addSetting("KioskMode", new KioskModeSetting(*this));
  addSetting("IndicateBlockedUI", new BlockingMainThreadIndication(*this, false));
  addSetting("WifiSetting", new WifiSetting(*this));
  addSetting("HighlightChangedParameters", new HighlightChangedParametersSetting(*this));
  addSetting("ForceHighlightChangedParameters", new ForceHighlightChangedParametersSetting(*this));
  addSetting("CrashOnError", new CrashOnError(*this));
  addSetting("TuneReference", new TuneReference(*this));
  addSetting("TotalRAM", new TotalRAM(*this));
  addSetting("UsedRAM", new UsedRAM(*this));
  addSetting("SyncVoiceGroups", new SyncVoiceGroupsAcrossUIS(*this));
  addSetting("ScreenSaverTimeout", new ScreenSaverTimeoutSetting(*this));
  addSetting("SyncSplit", new SplitPointSyncParameters(*this));
  addSetting("ExternalMidi", new ExternalMidiEnabledSetting(*this));
}

Settings::~Settings()
{
  save();
}

Settings::tUpdateID Settings::onChange(uint64_t flags)
{
  m_saveJob.trigger();
  return super::onChange(flags);
}

Glib::ustring Settings::getPrefix() const
{
  return m_actions->getBasePath().substr(1);
}

void Settings::init()
{
  load();

  for(auto &s : getSettings())
  {
    s.second->init();
  }
}

void Settings::reload()
{
  load();
}

void Settings::load()
{
  auto lock = m_isLoading.lock();

  DebugLevel::gassy(__PRETTY_FUNCTION__, G_STRLOC);

  try
  {
    DebugLevel::gassy(__PRETTY_FUNCTION__, G_STRLOC);
    FileInStream in(Application::get().getOptions()->getSettingsFile(), false);
    XmlReader reader(in, nullptr);
    reader.read<SettingsSerializer>(std::ref(*this));
  }
  catch(...)
  {
    DebugLevel::error("Exception loading the settings!");
  }

  DebugLevel::gassy(__PRETTY_FUNCTION__, G_STRLOC);

  sanitize();

  using nltools::msg::EndPoint;
  if(m_aeSettingsConnection.connected())
  {
    m_aeSettingsConnection.disconnect();
  }

  m_aeSettingsConnection = onConnectionEstablished(EndPoint::AudioEngine, [&] { sendGlobalAESettings(); });
}

void Settings::save()
{
  try
  {
    SettingsSerializer serializer(*this);
    XmlWriter writer(std::make_unique<FileOutStream>(Application::get().getOptions()->getSettingsFile(), false));
    serializer.write(writer, VersionAttribute::get());
  }
  catch(...)
  {
    nltools::Log::error("Could not save Settings to", Application::get().getOptions()->getSettingsFile());
  }
}

void Settings::sanitize()
{
  getSetting<DeviceName>()->sanitize();
}

void Settings::addSetting(const Glib::ustring &key, Setting *s)
{
  m_settings[key] = tSettingPtr(s);
}

const Settings::tMap &Settings::getSettings() const
{
  return m_settings;
}

Settings::tSettingPtr Settings::getSetting(const Glib::ustring &key)
{
  auto it = m_settings.find(key);

  if(it != m_settings.end())
    return it->second;

  return nullptr;
}

void Settings::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
  bool changed = knownRevision < getUpdateIDOfLastChange();

  writer.writeTag("settings", Attribute("changed", changed), [&]() {
    if(changed)
    {
      for(auto &setting : m_settings)
      {
        writer.writeTag(setting.first, [&]() { setting.second->writeDocument(writer, knownRevision); });
      }
    }
  });
}

void Settings::handleHTTPRequest(std::shared_ptr<NetworkRequest> request, const Glib::ustring &path)
{
  m_actions->handleRequest(request);
}

bool Settings::isLoading() const
{
  return m_isLoading.isLocked();
}

void Settings::sendSettingsToPlaycontroller(SendReason reason)
{
  if(reason == SendReason::HeartBeatDropped)
  {
    sendGlobalPlaycontrollerInitSettings();
    return;
  }
}

void Settings::sendGlobalPlaycontrollerInitSettings()
{
  getSetting<BaseUnitUIMode>()->syncExternals(SendReason::HeartBeatDropped);
  getSetting<RibbonRelativeFactor>()->syncExternals(SendReason::HeartBeatDropped);
  getSetting<ParameterEditModeRibbonBehaviour>()->syncExternals(SendReason::HeartBeatDropped);
  getSetting<VelocityCurve>()->syncExternals(SendReason::HeartBeatDropped);
  getSetting<AftertouchCurve>()->syncExternals(SendReason::HeartBeatDropped);
  getSetting<BenderCurve>()->syncExternals(SendReason::HeartBeatDropped);
  getSetting("Pedal1Type")->syncExternals(SendReason::HeartBeatDropped);
  getSetting("Pedal2Type")->syncExternals(SendReason::HeartBeatDropped);
  getSetting("Pedal3Type")->syncExternals(SendReason::HeartBeatDropped);
  getSetting("Pedal4Type")->syncExternals(SendReason::HeartBeatDropped);
}

void Settings::sendGlobalAESettings()
{
  getSetting<PresetGlitchSuppression>()->syncExternals(SendReason::HeartBeatDropped);
  getSetting<TransitionTime>()->syncExternals(SendReason::HeartBeatDropped);
  getSetting<NoteShift>()->syncExternals(SendReason::HeartBeatDropped);
  getSetting<TuneReference>()->syncExternals(SendReason::HeartBeatDropped);
}

void Settings::sendPresetSettingsToPlaycontroller()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto r1 = dynamic_cast<RibbonParameter *>(eb->findParameterByID({ C15::PID::Ribbon_1, VoiceGroup::Global }));
  auto r2 = dynamic_cast<RibbonParameter *>(eb->findParameterByID({ C15::PID::Ribbon_2, VoiceGroup::Global }));
  r1->sendModeToPlaycontroller();
  r2->sendModeToPlaycontroller();
}
