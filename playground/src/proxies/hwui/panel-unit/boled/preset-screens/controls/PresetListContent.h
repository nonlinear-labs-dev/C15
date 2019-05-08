#pragma once

#include <presets/PresetBank.h>
#include <proxies/hwui/controls/ControlWithChildren.h>
#include <sigc++/connection.h>
#include <functional>
#include <memory>

class EmptyBankLabel;
class Label;
class Application;
class PresetBank;
class PresetListEntry;

class PresetListContent : public ControlWithChildren
{
 private:
  typedef ControlWithChildren super;

 public:
  PresetListContent(const Rect &pos);
  virtual ~PresetListContent();

  void setup(shared_ptr<PresetBank> bank, int focussedPresetPos);
  bool animateSelectedPreset(function<void()> cb);

 private:
  void onBankSelectionChanged(shared_ptr<PresetBank> newBank);
  void onBankChanged(shared_ptr<PresetBank> bank);
  shared_ptr<Preset> getPresetAtPosition(shared_ptr<PresetBank> bank, int pos) const;

  connection m_onBankChanged;

  PresetListEntry *m_firstPreset = nullptr;
  PresetListEntry *m_secondPreset = nullptr;
  PresetListEntry *m_thirdPreset = nullptr;
  Label *m_emptyLabel = nullptr;
};
