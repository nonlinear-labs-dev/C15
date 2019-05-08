#pragma once

#include <proxies/hwui/OLEDProxy.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/InfoLayout.h>

class MultiLineLabel;

class SplashLayout : public InfoLayout
{
  typedef InfoLayout super;

 public:
  SplashLayout();
  virtual ~SplashLayout();

  static void setStatus(const std::string &msg);
  static void addStatus(const std::string &msg);

  void setMessage(const std::string &txt);
  void addMessage(const std::string &txt);

 private:
  virtual void addHeadline() override;
  virtual void addModuleCaption() override;
  virtual void addInfoLabel() override;
  virtual Scrollable *createScrollableContent() override;
  virtual Rect getScrollableAreaRect() const override;

  MultiLineLabel *m_message = nullptr;
  std::string m_text;
};
