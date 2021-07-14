#pragma once

#include "proxies/hwui/controls/Label.h"
#include <sigc++/connection.h>

class Application;
class Parameter;

class ParameterNameLabel : public Label
{
 private:
  typedef Label super;

 public:
  explicit ParameterNameLabel(const Rect &pos);
  ~ParameterNameLabel() override;

 protected:
  void setSuffixFontColor(FrameBuffer &fb) const override;

 private:
  ParameterNameLabel(const ParameterNameLabel &other);
  ParameterNameLabel &operator=(const ParameterNameLabel &);

  void onParameterSelected(Parameter *param);
  void onParameterChanged(const Parameter *param);

  void setFontColor(FrameBuffer &fb) const override;
  std::shared_ptr<Font> getFont() const override;
  int getFontHeight() const override;

  sigc::connection m_connection;

  void handleMCParameterName(const Parameter *pParameter);
  void handleParameterName(const Parameter *pParameter);
  Glib::ustring truncateMCName(bool changed, const Glib::ustring &name) const;
  void onPresetLoaded();
};
