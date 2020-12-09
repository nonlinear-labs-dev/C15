#pragma once
#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/controls/PNGControl.h>

class SplitParameterValue : public Label
{
 public:
  explicit SplitParameterValue(const Rect& pos);
  SplitParameterValue(const StringAndSuffix& text, const Rect& pos);

  bool redraw(FrameBuffer& fb) override;
  StringAndSuffix getText() const override;

 private:
  void init();
  void drawParts(FrameBuffer& fb, const std::vector<Glib::ustring>& parts);

  void onSplitIChanged(const Parameter* splitI);
  void onSplitIIChanged(const Parameter* splitII);

  double m_splitICP;
  double m_splitIICP;

 protected:
  virtual FrameBufferColors getColorForSplit(int i);
};
