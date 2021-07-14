#pragma once

#include <proxies/hwui/panel-unit/boled/info/InfoContent.h>

class Preset;
class Uuid;

class PresetInfoContent : public InfoContent
{
 private:
  typedef InfoContent super;

 public:
  PresetInfoContent();
  ~PresetInfoContent() override;

  void fillContents() override;
  static Glib::ustring createPresetTypeString(const Preset *preset);

 private:
  void onBankChanged(const Uuid &selectedBank);
  void onPresetSelectionChanged();

  void fillFromPreset(const Preset *preset);
  bool fillDefaults();
  static Preset *getCurrentPreset();
  void onPresetChanged();
  void connectToPreset(Preset *preset);

  sigc::connection m_bankConnection;
  sigc::connection m_presetConnection;
};
