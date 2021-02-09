#pragma once

#include "proxies/hwui/controls/ButtonMenu.h"

class PresetBank;
class Parameter;
class ParameterGroup;

class ParameterEditButtonMenu : public ButtonMenu
{
 private:
  typedef ButtonMenu super;

 public:
  ParameterEditButtonMenu(const Rect &rect);
  virtual ~ParameterEditButtonMenu();

  void selectButton(size_t i) override;

 private:
  void toggleGroupLock();
  void unlockAll();
  void lockAll();

  void onParameterSelectionChanged(Parameter* oldParameter, Parameter* newParameter, SignalOrigin signalType);
  void onGroupChanged();

  void sanitizeLastAction();

  ParameterGroup *m_currentGroup = nullptr;
  bool m_allParametersLocked = false;
  sigc::connection m_connection;

  static int s_lastAction;

 protected:
  void setup();
  virtual void addActions();
};
