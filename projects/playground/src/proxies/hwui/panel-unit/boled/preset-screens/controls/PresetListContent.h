#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <functional>

class Label;
class PresetListEntry;
class Preset;
class Bank;

class PresetListContent : public ControlWithChildren
{
 private:
  typedef ControlWithChildren super;

 public:
  PresetListContent(const Rect &pos);
  virtual ~PresetListContent();

  void setup(Bank *bank, size_t focussedPresetPos);
  bool animateSelectedPreset(std::function<void()> cb);

 protected:
  bool isTransparent() const override;

 private:
  void onEditBufferChanged();
  Preset *getPresetAtPosition(Bank *bank, int pos) const;

  PresetListEntry *m_firstPreset = nullptr;
  PresetListEntry *m_secondPreset = nullptr;
  PresetListEntry *m_thirdPreset = nullptr;
  Label *m_emptyLabel = nullptr;
};
