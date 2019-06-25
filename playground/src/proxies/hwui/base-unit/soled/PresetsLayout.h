#pragma once

#include <proxies/hwui/DFBLayout.h>
#include <tools/Uuid.h>

class Label;
class Setting;
class DirectLoadIndicator;

class PresetsLayout : public DFBLayout
{
 private:
  typedef DFBLayout super;

 public:
  PresetsLayout();
  virtual ~PresetsLayout();

 private:
  void onBankSelected(const Uuid &selectedBank);
  void onBankChanged();
  void onPresetLoaded();
  void onAutoLoadSettingChanged(const Setting *s);
  void onEditBufferModified(bool b);
  void update();
  Uuid getCurrentlySelectedPresetUUID() const;
  void updateDirectLoadLabel();
  bool updateNameAndNumber();
  Glib::ustring formatBankAndPresetNumber(size_t bankNumber, size_t presetPosition, bool modified) const;

  sigc::connection m_connection;
  Label *m_number;
  Label *m_name;
  DirectLoadIndicator *m_directLoad;
};
