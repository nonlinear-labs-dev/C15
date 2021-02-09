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
  ParameterNameLabel(const Rect &pos);
  virtual ~ParameterNameLabel();

  void updateParameter(const Parameter *parameter);

 protected:
  virtual void setSuffixFontColor(FrameBuffer &fb) const;

 private:
  ParameterNameLabel(const ParameterNameLabel &other);
  ParameterNameLabel &operator=(const ParameterNameLabel &);

  void onParameterSelected(Parameter *param, SignalOrigin signalType);
  void onParameterChanged(const Parameter *param);

  virtual void setFontColor(FrameBuffer &fb) const override;
  virtual std::shared_ptr<Font> getFont() const override;
  virtual int getFontHeight() const override;

  sigc::connection m_connection;

  void handleMCParameterName(const Parameter *pParameter);

  void handleParameterName(const Parameter *pParameter);

  Glib::ustring truncateMCName(const bool changed, const Glib::ustring &name) const;

  void onPresetLoaded();
};
