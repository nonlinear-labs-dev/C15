#pragma once

#include "PresetLabel.h"

class Preset;

class PresetNameLabel : public PresetLabel
{
 private:
  typedef PresetLabel super;

 public:
  explicit PresetNameLabel(const Rect &pos);
  ~PresetNameLabel() override = default;

  void drawBackground(FrameBuffer &fb) override;
  void update(const Glib::ustring &presetName, bool selected, bool loaded);

 protected:
  Font::Justification getJustification() const override;
  int getXOffset() const override;
  int getRightMargin() const override;
  StringAndSuffix shortenStringIfNeccessary(const std::shared_ptr<Font> &font,
                                            const StringAndSuffix &text) const override;
};
