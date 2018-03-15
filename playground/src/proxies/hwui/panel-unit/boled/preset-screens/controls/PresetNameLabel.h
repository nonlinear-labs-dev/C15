#pragma once

#include "PresetLabel.h"

class Application;
class PresetBank;
class Preset;

class PresetNameLabel : public PresetLabel
{
  private:
    typedef PresetLabel super;

  public:
    PresetNameLabel (const Rect &pos);
    virtual ~PresetNameLabel ();

    void drawBackground (FrameBuffer &fb) override;
    void update(const Glib::ustring &presetName, bool selected, bool loaded);

  protected:
    Font::Justification getJustification () const override;
    int getXOffset () const override;
    int getRightMargin() const override;
    Glib::ustring shortenStringIfNeccessary (shared_ptr<Font> font, const Glib::ustring &text) const override;
};

