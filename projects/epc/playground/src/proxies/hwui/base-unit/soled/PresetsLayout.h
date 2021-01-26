#pragma once

#include <proxies/hwui/Layout.h>
#include <tools/Uuid.h>
#include <sigc++/connection.h>

class Label;
class Setting;
class DirectLoadIndicator;

class PresetsLayout : public Layout
{
 private:
  typedef Layout super;

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
  Label *m_type;
  DirectLoadIndicator *m_directLoad;
};
