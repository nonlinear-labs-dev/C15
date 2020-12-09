#include "BasicItemWithValueText.h"
#include "controls/CaptionLabel.h"
#include "LeftAligned9pxCaptionLabel.h"

BasicItemWithValueText::BasicItemWithValueText(const Glib::ustring &label, const Glib::ustring &value, const Rect &r)
    : BasicItem(label, r)
{
  auto rightHalf = r;
  const auto totalWidth = rightHalf.getWidth() / 2;
  const auto indicationWidth = 10;
  const auto labelWidth = totalWidth - indicationWidth;

  rightHalf.setTop(r.getTop() + 1);
  rightHalf.setWidth(labelWidth);
  rightHalf.setLeft(r.getLeft() + labelWidth + indicationWidth);
  m_valueLabel = addControl(new CaptionLabel<LeftAligned9pxCaptionLabel>(value, rightHalf, true, false));

  auto indicationPos = Rect { rightHalf.getRight(), rightHalf.getTop(), indicationWidth, rightHalf.getHeight() - 1 };
  m_enterLabel = addControl(new Label("...", indicationPos));
}
