#include "BankXOfYLabel.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/Bank.h"

BankXOfYLabel::BankXOfYLabel(const Rect &pos)
    : super(pos)
{
  Application::get().getPresetManager()->onBankSelection(sigc::mem_fun(this, &BankXOfYLabel::onBankSelectionChanged));
}

BankXOfYLabel::~BankXOfYLabel()
{
}

void BankXOfYLabel::onBankSelectionChanged(const Uuid &selectedBank)
{
  if(auto newBank = Application::get().getPresetManager()->getSelectedBank())
  {
    auto pos = Application::get().getPresetManager()->getBankPosition(newBank->getUuid());
    auto bankNumber = to_string(pos + 1);
    auto numBanks = to_string(Application::get().getPresetManager()->getNumBanks());
    setText({ bankNumber + " of " + numBanks });
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
