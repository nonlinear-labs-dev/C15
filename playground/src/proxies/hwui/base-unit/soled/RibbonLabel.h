#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;

class RibbonLabel : public Label
{
 private:
  typedef Label super;

 public:
  RibbonLabel(int paramID, const Rect &rect);
  virtual ~RibbonLabel();

  Glib::ustring getText() const override;

 private:
  RibbonLabel(const RibbonLabel &other) = delete;
  RibbonLabel &operator=(const RibbonLabel &) = delete;

  Glib::ustring crop(const Glib::ustring &text) const;
  Glib::ustring binarySearchLength(const Glib::ustring &text, int min, int max) const;

  int m_parameterID;
};
