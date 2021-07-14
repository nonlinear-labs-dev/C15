#include <proxies/hwui/TextCropper.h>
#include "BankNameLabel.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include <presets/Bank.h>
#include <proxies/hwui/FrameBuffer.h>

BankNameLabel::BankNameLabel(const Rect &pos)
    : super(pos)
{
}

BankNameLabel::~BankNameLabel()
{
}

void BankNameLabel::updateLabel(Bank *newBank)
{
  if(newBank)
  {
    auto pm = Application::get().getPresetManager();
    auto bankName = newBank->getName(true);
    auto pos = pm->getBankPosition(newBank->getUuid()) + 1;
    auto s = to_string(pos) + ": " + bankName;
    setText({ s });
  }
  else
  {
    setText("");
  }
}

StringAndSuffix BankNameLabel::shortenStringIfNeccessary(const std::shared_ptr<Font> &font,
                                                         const StringAndSuffix &text) const
{
  auto availableWidth = getPosition().getWidth() - 2 * getXOffset();
  return TextCropper::shortenStringIfNeccessary(font, text.text, availableWidth);
}

void BankNameLabel::setBackgroundColor(FrameBuffer &fb) const
{
  if(isHighlight())
    fb.setColor(FrameBufferColors::C255);
  else
    fb.setColor(FrameBufferColors::C103);
}

void BankNameLabel::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBufferColors::C43);
}

Font::Justification BankNameLabel::getJustification() const
{
  return Font::Justification::Center;
}

int BankNameLabel::getXOffset() const
{
  return 2;
}

BankMidiLabel::BankMidiLabel(const Rect &pos)
    : super(pos)
{
}

BankMidiLabel::~BankMidiLabel() = default;

void BankMidiLabel::updateLabel(Bank *newBank)
{
  if(newBank->isMidiSelectedBank())
    setText({ "\uE0C1", 0 });
  else
    setText({ "", 0 });
}

int BankMidiLabel::getYOffset() const
{
  return -1;
}

int BankMidiLabel::getXOffset() const
{
  return 3;
}
