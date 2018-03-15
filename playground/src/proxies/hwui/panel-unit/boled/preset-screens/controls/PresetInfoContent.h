#pragma once

#include <presets/PresetBank.h>
#include <proxies/hwui/panel-unit/boled/InfoContent.h>

class MultiLineLabel;
class Label;
class PresetBank;

class PresetInfoContent : public InfoContent
{
  private:
    typedef InfoContent super;

  public:
    PresetInfoContent ();
    virtual ~PresetInfoContent ();

  private:
    void onBankChanged (shared_ptr<PresetBank> bank);
    void onPresetSelectionChanged ();

    void fixLayout();
    bool fillFromPreset (const Preset *preset);
    bool fillDefaults ();
    Preset *getCurrentPreset ();
    void onPresetChanged();
    void connectToPreset (Preset* preset);

    sigc::connection m_bankConnection;
    sigc::connection m_presetConnection;
};

