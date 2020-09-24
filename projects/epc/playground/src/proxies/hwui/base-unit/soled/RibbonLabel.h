#pragma once

#include "proxies/hwui/controls/Label.h"
#include <ParameterId.h>

class Application;

class RibbonLabel : public Label
{
 private:
  typedef Label super;

 public:
  RibbonLabel(const ParameterId &paramID, const Rect &rect);
  virtual ~RibbonLabel();

  StringAndSuffix getText() const override;

 private:
  RibbonLabel(const RibbonLabel &other) = delete;
  RibbonLabel &operator=(const RibbonLabel &) = delete;

  Glib::ustring crop(const Glib::ustring &text) const;
  Glib::ustring binarySearchLength(const Glib::ustring &text, int min, int max) const;

  ParameterId m_parameterID;
};
