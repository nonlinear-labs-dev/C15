#pragma once
#include <proxies/hwui/controls/ControlWithChildren.h>
#include <nltools/Types.h>
#include <Application.h>
#include <proxies/hwui/HWUI.h>

class Parameter;
class MacroControlParameter;
class LabelRegular8;

class MCAssignedIndicator : public ControlWithChildren
{
 public:
  MCAssignedIndicator(const Rect& r, const Parameter* p);
  bool redraw(FrameBuffer& fb) override;
  void drawBackground(FrameBuffer& fb) override;

 private:
  struct AffectedGroups
  {
    std::array<bool, 2> mono { false, false };
    std::array<bool, 2> unison { false, false };
    std::array<bool, 2> part { false, false };
    bool scale { false };
    bool master { false };

    VoiceGroup currentHWUIVG;

    [[nodiscard]] bool affected(SoundType s) const
    {
      auto currentVG = Application::get().getHWUI()->getCurrentVoiceGroup();
      int partIdx = currentVG == VoiceGroup::I ? 0 : 1;

      switch(s)
      {
        case SoundType::Single:
        default:
          return scale || master || mono[0] || unison[0];
        case SoundType::Layer:
          return scale || master || mono[0] || unison[0] || part[partIdx];
        case SoundType::Split:
          return scale || master || mono[partIdx] || unison[partIdx] || part[partIdx];
      }
    }
  };

  void drawSplit(FrameBuffer& fb, const AffectedGroups& mods);
  void drawLayer(FrameBuffer& fb, const AffectedGroups& mods);
  void drawSingle(FrameBuffer& fb, const AffectedGroups& mods);

  LabelRegular8* m_middleRowLabel = nullptr;

  const MacroControlParameter* m_parameter = nullptr;
  void drawNonLEDTargets(FrameBuffer& fb);
  void drawArrow(FrameBuffer& fb) const;
  void onVoiceGroupChanged(VoiceGroup vg);
};
