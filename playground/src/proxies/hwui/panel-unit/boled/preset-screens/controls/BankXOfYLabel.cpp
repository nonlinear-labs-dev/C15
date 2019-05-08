#include "BankXOfYLabel.h"
#include "Application.h"
#include "presets/PresetManager.h"

BankXOfYLabel::BankXOfYLabel(const Rect &pos)
    : super(pos)
{
  Application::get().getPresetManager()->onBankSelection(sigc::mem_fun(this, &BankXOfYLabel::onBankSelectionChanged));
}

BankXOfYLabel::~BankXOfYLabel()
{
}

void BankXOfYLabel::onBankSelectionChanged(shared_ptr<PresetBank> newBank)
{
  if(newBank)
  {
    auto bankNumber = to_string(Application::get().getPresetManager()->calcOrderNumber(newBank.get()));
    auto numBanks = to_string(Application::get().getPresetManager()->getNumBanks());
    setText(bankNumber + " of " + numBanks);
  }
  else
    setText("");
}

void BankXOfYLabel::setBackgroundColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C43);
}

void BankXOfYLabel::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C179);
}

Font::Justification BankXOfYLabel::getJustification() const
{
  return Font::Justification::Right;
}
