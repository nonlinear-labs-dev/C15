#pragma once

#include <proxies/hwui/DFBLayout.h>

class Overlay;
class Carousel;
class Rect;
class ButtonMenu;
class Parameter;

class ParameterLayout2 : public DFBLayout
{
  public:
    typedef DFBLayout super;
    typedef ParameterLayout2 virtual_base;
    ParameterLayout2 ();

    constexpr static int BUTTON_VALUE_Y_POSITION = 34;
    constexpr static int BIG_SLIDER_X = 77;
    constexpr static int BIG_SLIDER_WIDTH = 102;

  protected:
    virtual Parameter * getCurrentParameter () const;
    virtual Parameter * getCurrentEditParameter () const;
    virtual bool onButton (Buttons i, bool down, ButtonModifiers modifiers) override;
    virtual bool onRotary (int inc, ButtonModifiers modifiers) override;
    virtual void setDefault ();
};

class ParameterSelectLayout2 : public virtual ParameterLayout2
{
  public:
    typedef ParameterLayout2 super;
    ParameterSelectLayout2 ();

  protected:
    virtual void init () override;
    virtual bool onButton (Buttons i, bool down, ButtonModifiers modifiers) override;
    virtual Carousel *createCarousel (const Rect &rect);

    void setCarousel(Carousel *c);
    Carousel *getCarousel();

  private:
    Carousel *m_carousel = nullptr;
};

class ParameterEditLayout2 : public virtual ParameterLayout2
{
  public:
    typedef ParameterLayout2 super;
    ParameterEditLayout2 ();
    virtual ~ParameterEditLayout2 ();

  protected:
    virtual void init () override;
    virtual bool onButton (Buttons i, bool down, ButtonModifiers modifiers) override;
    virtual ButtonMenu *createMenu (const Rect &rect) = 0;

    ButtonMenu *getMenu ();

  private:
    ButtonMenu *m_menu = nullptr;
};
