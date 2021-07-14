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
  ~RibbonLabel() override;

  StringAndSuffix getText() const override;

  RibbonLabel(const RibbonLabel &other) = delete;
  RibbonLabel &operator=(const RibbonLabel &) = delete;

 private:
  Glib::ustring crop(const Glib::ustring &text) const;
  Glib::ustring binarySearchLength(const Glib::ustring &text, int min, int max) const;

  ParameterId m_parameterID;
};
