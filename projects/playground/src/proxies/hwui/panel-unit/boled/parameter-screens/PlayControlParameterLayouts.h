#pragma once

#include "ParameterLayout.h"
class PlayControlParameterLayout2 : public virtual ParameterLayout2
{
 public:
  typedef ParameterLayout2 super;

  PlayControlParameterLayout2();

  ~PlayControlParameterLayout2() override;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;

  bool onRotary(int inc, ButtonModifiers modifiers) override;

 protected:
  void onInit() override;

  enum Mode : uint8_t
  {
    ValueEdit = 0,
    Select = 1,
  };

  virtual void toggleMode(uint8_t desiredMode);

  virtual bool isModeSupported(uint8_t desiredMode) const;

  virtual void setMode(uint8_t desiredMode);

  uint8_t getMode() const;

 protected:
  static uint8_t s_mode;
  static int s_instanceCount;
};

class RibbonParameterLayout2 : public virtual PlayControlParameterLayout2
{
 public:
  typedef PlayControlParameterLayout2 super;

  RibbonParameterLayout2();
  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
};

class PedalParameterLayout2 : public virtual PlayControlParameterLayout2
{
 public:
  typedef PlayControlParameterLayout2 super;

  PedalParameterLayout2();

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;

 protected:
  enum Mode : uint8_t
  {
    Behaviour = 2
  };
};

class AftertouchParameterLayout2 : public virtual PlayControlParameterLayout2
{
 public:
  typedef PlayControlParameterLayout2 super;

  AftertouchParameterLayout2();
};

class PitchbendParameterLayout2 : public virtual PlayControlParameterLayout2
{
 public:
  typedef PlayControlParameterLayout2 super;

  PitchbendParameterLayout2();
};

class PlayControlParameterSelectLayout2 : public ParameterSelectLayout2
{
 public:
  typedef ParameterSelectLayout2 super;

  PlayControlParameterSelectLayout2();

 protected:
  ModuleCaption *createModuleCaption() const override;

 public:
  Carousel *createCarousel(const Rect &rect) override;
  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
};

class PlayControlParameterEditLayout2 : public ParameterEditLayout2
{
 public:
  typedef ParameterEditLayout2 super;

  PlayControlParameterEditLayout2();

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
};

class PlayControlWithBehaviourEditLayout2 : public PlayControlParameterEditLayout2,
                                            public virtual PlayControlParameterLayout2
{
 public:
  typedef PlayControlParameterEditLayout2 super1;
  typedef PlayControlParameterLayout2 super2;

  PlayControlWithBehaviourEditLayout2();

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  bool onRotary(int inc, ButtonModifiers modifiers) override;

  void setMode(uint8_t desiredMode) override;
};

class RibbonParameterSelectLayout2 : public PlayControlParameterSelectLayout2, public RibbonParameterLayout2
{
 public:
  typedef PlayControlParameterSelectLayout2 super1;
  typedef RibbonParameterLayout2 super2;

  RibbonParameterSelectLayout2();

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  bool onRotary(int inc, ButtonModifiers modifiers) override;

 protected:
  bool isModeSupported(uint8_t desiredMode) const override;
  void setMode(uint8_t desiredMode) override;

 private:
  enum Mode : uint8_t
  {
    Behaviour = 2,
  };
};

class PedalParameterSelectLayout2 : public PlayControlParameterSelectLayout2, public PedalParameterLayout2
{
 public:
  typedef PlayControlParameterSelectLayout2 super1;
  typedef PedalParameterLayout2 super2;

  PedalParameterSelectLayout2();

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  bool onRotary(int inc, ButtonModifiers modifiers) override;

 protected:
  bool isModeSupported(uint8_t desiredMode) const override;
  void setMode(uint8_t desiredMode) override;

 private:
  enum Mode : uint8_t
  {
    Behaviour = 2,
  };
};

class AftertouchParameterSelectLayout2 : public PlayControlParameterSelectLayout2, public AftertouchParameterLayout2
{
 public:
  typedef PlayControlParameterSelectLayout2 super1;
  typedef AftertouchParameterLayout2 super2;

  AftertouchParameterSelectLayout2();

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
};

class AftertouchParameterEditLayout2 : public PlayControlParameterEditLayout2, public AftertouchParameterLayout2
{
 public:
  typedef PlayControlParameterEditLayout2 super1;
  typedef AftertouchParameterLayout2 super2;

  AftertouchParameterEditLayout2();

  ButtonMenu *createMenu(const Rect &rect) override;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
};

class PitchbendParameterSelectLayout2 : public PlayControlParameterSelectLayout2, public PitchbendParameterLayout2
{
 public:
  typedef PlayControlParameterSelectLayout2 super1;
  typedef PitchbendParameterLayout2 super2;

  PitchbendParameterSelectLayout2();

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
};

class PitchbendParameterEditLayout2 : public PlayControlParameterEditLayout2, public PitchbendParameterLayout2
{
 public:
  typedef PlayControlParameterEditLayout2 super1;
  typedef PitchbendParameterLayout2 super2;

  PitchbendParameterEditLayout2();

  ButtonMenu *createMenu(const Rect &rect) override;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
};

class RibbonParameterEditLayout2 : public PlayControlWithBehaviourEditLayout2, public RibbonParameterLayout2
{
 public:
  typedef PlayControlWithBehaviourEditLayout2 super1;
  typedef RibbonParameterLayout2 super2;

  RibbonParameterEditLayout2();

  ButtonMenu *createMenu(const Rect &rect) override;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;

  bool onRotary(int inc, ButtonModifiers modifiers) override;

 protected:
  bool isModeSupported(uint8_t desiredMode) const override;

  void setMode(uint8_t desiredMode) override;
};

class PedalParameterEditLayout2 : public PlayControlWithBehaviourEditLayout2, public PedalParameterLayout2
{
 public:
  typedef PlayControlWithBehaviourEditLayout2 super1;
  typedef PedalParameterLayout2 super2;

  PedalParameterEditLayout2();

  ButtonMenu *createMenu(const Rect &rect) override;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;

  bool onRotary(int inc, ButtonModifiers modifiers) override;
};
