#pragma once

#include <proxies/hwui/Layout.h>
#include <sigc++/connection.h>
#include <nltools/threading/Throttler.h>

class Overlay;
class Carousel;
class Rect;
class ButtonMenu;
class Parameter;
class Slider;
class Label;
class Button;
class ModuleCaption;
class EditBuffer;

class ParameterLayout2 : public Layout
{
 public:
  typedef Layout super;
  typedef ParameterLayout2 virtual_base;
  ParameterLayout2();

 protected:
  void init() override;
  constexpr static int BUTTON_VALUE_Y_POSITION = 34;
  constexpr static int BIG_SLIDER_X = 77;
  constexpr static int BIG_SLIDER_WIDTH = 102;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  bool onRotary(int inc, ButtonModifiers modifiers) override;
  void copyFrom(Layout *src) override;

  virtual Parameter *getCurrentParameter() const;
  virtual Parameter *getCurrentEditParameter() const;
  virtual void setDefault();
  virtual void handlePresetValueRecall();
  virtual Control *createParameterValueControl();
  void onSoundTypeChanged();
  virtual ModuleCaption *createModuleCaption() const;

 private:
  Throttler m_soundTypeRedrawThrottler;
  void showRecallScreenIfAppropriate();
};

class ParameterSelectLayout2 : public virtual ParameterLayout2
{
 public:
  typedef ParameterLayout2 super;
  ParameterSelectLayout2();

 protected:
  void init() override;
  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;

  virtual Carousel *createCarousel(const Rect &rect);

  void setCarousel(Carousel *c);
  Carousel *getCarousel();

 private:
  Carousel *m_carousel = nullptr;
};

class ParameterEditLayout2 : public virtual ParameterLayout2
{
 public:
  typedef ParameterLayout2 super;
  ParameterEditLayout2();
  ~ParameterEditLayout2() override;

 protected:
  void init() override;
  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  virtual ButtonMenu *createMenu(const Rect &rect) = 0;

  ButtonMenu *getMenu();

 private:
  ButtonMenu *m_menu = nullptr;
};

class ParameterRecallLayout2 : public virtual ParameterLayout2
{
 public:
  typedef ParameterLayout2 super;
  ParameterRecallLayout2();
  ~ParameterRecallLayout2() override;

 protected:
  void init() override;
  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  bool onRotary(int inc, ButtonModifiers modifiers) override;
  ButtonMenu *createMenu(const Rect &rect);

 protected:
  void doRecall();
  void undoRecall();
  virtual void updateUI(bool paramLikeInPreset);
  void onParameterChanged(const Parameter *);
  void onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam);

  Slider *m_slider;
  Label *m_leftValue;
  Label *m_rightValue;
  Button *m_buttonC;
  Button *m_buttonB;
  tControlPositionValue m_recallValue;
  Button *m_buttonA;
  bool m_paramLikeInPreset;
  Button *m_buttonD;
  Glib::ustring m_recallString;
  sigc::connection m_paramConnection;
};

class PartMasterRecallLayout2 : public ParameterRecallLayout2
{
 public:
  PartMasterRecallLayout2();
  ~PartMasterRecallLayout2() override;

 protected:
  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  void onMuteChanged();
  void updateUI(bool paramLikeInPreset) override;

  bool shouldShowNormalRecallScreen() const;

 private:
  void toggleMute() const;
  Parameter *const m_muteParameter;
  sigc::connection m_muteParameterConnection;
};
