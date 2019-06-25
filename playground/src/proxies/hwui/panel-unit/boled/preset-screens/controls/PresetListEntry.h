#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <sigc++/connection.h>
#include <memory>

class Preset;
class PresetNameLabel;
class PresetNumberLabel;

class PresetLabel;
class Application;

class PresetListEntry : public ControlWithChildren
{
 private:
  typedef ControlWithChildren super;

 public:
  using tCallback = std::function<void()>;

  PresetListEntry(const Rect &pos);
  ~PresetListEntry() override;

  void setPreset(Preset *preset, bool selected);
  bool redraw(FrameBuffer &fb) override;
  void animate(tCallback cb);

 private:
  bool animationProgress();
  void doAnimationCallback();
  void onPresetChanged(const Preset *preset);

  float m_animationProgress;
  tCallback m_animationCallback;

  PresetNumberLabel *m_number = nullptr;
  PresetNameLabel *m_name = nullptr;

  connection m_animationConnection;
  connection m_presetConnection;

  bool m_selected = false;
};
