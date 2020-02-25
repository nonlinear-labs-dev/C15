#include "BasicItemWithValueText.h"
#include "controls/CaptionLabel.h"
#include "LeftAligned9pxCaptionLabel.h"

BasicItemWithValueText::BasicItemWithValueText(const Glib::ustring &label, const Glib::ustring &value, const Rect &r)
    : BasicItem(label, r)
{
  auto rightHalf = r;
  rightHalf.setTop(r.getTop() + 1);
  rightHalf.setWidth(rightHalf.getWidth() / 2);
  rightHalf.setLeft(r.getLeft() + rightHalf.getWidth());
  addControl(new CaptionLabel<LeftAligned9pxCaptionLabel>(value, rightHalf, true, false));
}
