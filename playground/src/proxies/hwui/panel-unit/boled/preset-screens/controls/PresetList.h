#pragma once

#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListBase.h>
#include <tools/Uuid.h>

class Preset;

class PresetList : public PresetListBase
{
  using super = PresetListBase;

 public:
  PresetList(const Rect& pos, bool showBankArrows);
  ~PresetList() override;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  void onRotary(int inc, ButtonModifiers modifiers) override;

  std::pair<size_t, size_t> getSelectedPosition() const override;

private:
  void onBankSelectionChanged(const Uuid& selectedBank);
  void onBankChanged();
  void onEditBufferChanged();

  sigc::connection m_bankChangedConnection;
  Uuid m_uuidOfLastLoadedPreset;
};

// Make this the new class:
//Replace Whole PresetListBase with generic traversal of Presets instead of strong coupling with selection

class GenericPresetList : public ControlWithChildren
{
 public:
  explicit GenericPresetList(const Point& p);
  virtual ~GenericPresetList();

  void incBankSelection();
  void decBankSelection();
  void incPresetSelection();
  void decPresetSelection();

  virtual void action() = 0;

  Preset* getPresetAtSelected() const;
  bool redraw(FrameBuffer& fb) override;

  sigc::connection onChange(sigc::slot<void, GenericPresetList*> pl);

 protected:
  void signalChanged();

  void drawPresets(FrameBuffer& fb, Preset* middle);
  virtual bool sanitizePresetPtr();

  Preset* m_selectedPreset = nullptr;
  sigc::connection m_numBanks;
  sigc::connection m_restoreHappened;

  sigc::signal<void, GenericPresetList*> m_signalChanged;

};

class PresetListVGSelect : public GenericPresetList
{
 public:
  explicit PresetListVGSelect(const Point& p);
  void action() override;
};