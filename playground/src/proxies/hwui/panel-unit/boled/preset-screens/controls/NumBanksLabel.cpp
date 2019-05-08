#include "NumBanksLabel.h"
#include "Application.h"
#include <presets/PresetManager.h>

NumBanksLabel::NumBanksLabel(const Rect& pos)
    : super(pos)
{
  Application::get().getPresetManager()->onNumBanksChanged(mem_fun(this, &NumBanksLabel::onNumBanksChanged));
}

NumBanksLabel::~NumBanksLabel()
{
}

void NumBanksLabel::onNumBanksChanged(int numBanks)
{
  setText(UNDO::StringTools::buildString('[', numBanks, ']'));
}
