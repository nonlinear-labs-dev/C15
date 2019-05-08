#include "BankNumberLabel.h"
#include "Application.h"
#include "presets/PresetManager.h"

BankNumberLabel::BankNumberLabel(const Rect &pos)
    : super(pos)
{
  Application::get().getPresetManager()->onBankSelection(sigc::mem_fun(this, &BankNumberLabel::onBankSelectionChanged));
}

BankNumberLabel::~BankNumberLabel()
{
}

void BankNumberLabel::onBankSelectionChanged(shared_ptr<PresetBank> newBank)
{
  if(newBank)
    setText(to_string(Application::get().getPresetManager()->calcOrderNumber(newBank.get())));
  else
    setText("");
}

void BankNumberLabel::setBackgroundColor(FrameBuffer &fb) const
{
  if(isHighlight())
    fb.setColor(FrameBuffer::Colors::C255);
  else
    fb.setColor(FrameBuffer::Colors::C103);
}

void BankNumberLabel::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C43);
}

Font::Justification BankNumberLabel::getJustification() const
{
  return Font::Justification::Right;
}
