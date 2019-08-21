#pragma once
#include <proxies/hwui/controls/ControlWithChildren.h>

class Preset;

class GenericPresetList : public ControlWithChildren
{
 public:
  explicit GenericPresetList(const Point& p);
  ~GenericPresetList() override;

  void incBankSelection();
  void decBankSelection();
  void incPresetSelection();
  void decPresetSelection();

  virtual void action() = 0;

  Preset* getPresetAtSelected() const;
  bool redraw(FrameBuffer& fb) override;

  sigc::connection onChange(const sigc::slot<void, GenericPresetList*>& pl);

 protected:
  void signalChanged();

  void drawPresets(FrameBuffer& fb, Preset* middle);
  virtual bool sanitizePresetPtr();

  Preset* m_selectedPreset = nullptr;
  sigc::connection m_numBanks;
  sigc::connection m_restoreHappened;

  sigc::signal<void, GenericPresetList*> m_signalChanged;
};