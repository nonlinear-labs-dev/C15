#pragma once
#include "proxies/hwui/controls/Rect.h"
#include "proxies/hwui/controls/Control.h"
#include <tools/StringAndSuffix.h>

class CaptionLabelBase
{
 public:
  CaptionLabelBase(bool changeHighlight, bool changeBackground);
  virtual ~CaptionLabelBase();

  virtual bool setText(const std::string&) = 0;

 protected:
  [[nodiscard]] virtual int getXOffset() const;
  virtual void setFontColor(FrameBuffer& fb) const;
  virtual void setBackgroundColor(FrameBuffer& fb) const;

  bool m_changeHighlight;
  bool m_changeHighlightBackground;
};

template <typename tLabelType> class CaptionLabel : public tLabelType, public CaptionLabelBase
{
 public:
  CaptionLabel(const Glib::ustring& caption, const Rect& rect, bool changeHighlight, bool changeBackground)
      : tLabelType(caption, rect)
      , CaptionLabelBase(changeHighlight, changeBackground)
  {
  }

  bool setText(const std::string& s) override
  {
    return tLabelType::setText(StringAndSuffix { s });
  }

 protected:
  [[nodiscard]] int getXOffset() const override
  {
    return CaptionLabelBase::getXOffset();
  }

  void setFontColor(FrameBuffer& fb) const override
  {
    tLabelType::setFontColor(fb);
    CaptionLabelBase::setFontColor(fb);
  }

  void setBackgroundColor(FrameBuffer& fb) const override
  {
    tLabelType::setBackgroundColor(fb);
    CaptionLabelBase::setBackgroundColor(fb);
  }
};
