#pragma once

#include "proxies/hwui/controls/LabelRegular8.h"

class MiniParameterLabel : public LabelRegular8
{
 private:
  typedef LabelRegular8 super;

 public:
  // public methods
  MiniParameterLabel(const Glib::ustring &text, const Rect &pos);
  virtual ~MiniParameterLabel();

  void setSelected(bool selected);
  bool isSelected() const;

 private:
  // private methods
  MiniParameterLabel(const MiniParameterLabel &other);
  MiniParameterLabel &operator=(const MiniParameterLabel &);

  bool redraw(FrameBuffer &fb) override;
  void setFontColor(FrameBuffer &fb) const override;

  bool m_selected;
};
