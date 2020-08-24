#pragma once
#include <proxies/hwui/controls/ControlWithChildren.h>
#include <nltools/Types.h>

class Parameter;
class MacroControlParameter;
class LabelRegular8;

class MCAssignedIndicator : public ControlWithChildren
{
 public:
  MCAssignedIndicator(const Rect& r, const Parameter* p);
  bool redraw(FrameBuffer& fb) override;

 private:
  struct AffectedGroups
  {
    std::array<bool, 2> mono { false, false };
    std::array<bool, 2> unison { false, false };
    std::array<bool, 2> part { false, false };
    bool scale { false };
    bool master { false };

    [[nodiscard]] bool affected(SoundType s) const
    {
      switch(s)
      {
        case SoundType::Single:
        default:
          return scale || master || mono[0] || unison[0];
        case SoundType::Layer:
          return scale || master || mono[0] || unison[0] || part[0] || part[1];
        case SoundType::Split:
          return scale || master || mono[0] || mono[1] || unison[0] || unison[1] || part[0] || part[1];
      }
    }
  };

  void drawSplit(FrameBuffer& fb, const AffectedGroups& mods);
  void drawLayer(FrameBuffer& fb, const AffectedGroups& mods);
  void drawSingle(FrameBuffer& fb, const AffectedGroups& mods);

  LabelRegular8* m_topRowLabel = nullptr;
  LabelRegular8* m_middleRowLabel = nullptr;
  LabelRegular8* m_bottomRowLabel = nullptr;
  const MacroControlParameter* m_parameter = nullptr;
  void drawNonLEDTargets(FrameBuffer& fb);
  void drawArrow(FrameBuffer& fb) const;
};
