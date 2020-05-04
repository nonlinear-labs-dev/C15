#pragma once
#include <proxies/hwui/controls/Label.h>
#include <sigc++/connection.h>

class Parameter;

class MuteIndicator : public Label
{
 public:
  explicit MuteIndicator(const Rect& r);
  ~MuteIndicator() override;

  std::shared_ptr<Font> getFont() const override;

  int getFontHeight() const override;

 private:
  void onParameterChanged(const Parameter* p);
  sigc::connection m_parameterConnection;
  sigc::connection m_vgConnection;
  sigc::connection m_soundTypeConnection;
  void setup();
};
