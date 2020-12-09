#pragma once
#include "BasicItem.h"

class CaptionLabelBase;
class Label;

class BasicItemWithValueText : public BasicItem
{
 public:
  BasicItemWithValueText(const Glib::ustring& label, const Glib::ustring& value, const Rect& r);

 protected:
  CaptionLabelBase* m_valueLabel;
  Label* m_enterLabel;
};
