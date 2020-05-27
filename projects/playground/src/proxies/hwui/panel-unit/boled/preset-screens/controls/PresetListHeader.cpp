#include <Application.h>
#include <presets/Bank.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/BankNameLabel.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListHeader.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListHeaderArrow.h>

PresetListHeader::PresetListHeader(const Rect &pos, bool showBankArrows)
    : super(pos)
    , m_showBankArrows(showBankArrows)
{
}

PresetListHeader::~PresetListHeader()
{
}

void PresetListHeader::setup(Bank *bank)
{
  if(m_showBankArrows && bank)
  {
    setupWithArrows(bank);
  }
  else
  {
    addControl(new BankNameLabel(Rect(0, 0, getPosition().getWidth(), 15)));
  }
}

void PresetListHeader::setupWithArrows(Bank *bank)
{
  clear();

  auto pm = Application::get().getPresetManager();

  const Rect &pos = getPosition();
  const int arrowWidth = 11;
  const int arrowHeight = 14;

  auto order = pm->getBankPosition(bank->getUuid());
  auto isFirst = order == 0;
  auto isLast = order == pm->getNumBanks() - 1;

  addControl(new PresetListHeaderArrow(isFirst ? " " : "\uE303", Rect(0, -1, arrowWidth, arrowHeight), 0));
  addControl(new BankNameLabel(Rect(arrowWidth, 0, pos.getWidth() - 2 * arrowWidth, 13)))->updateLabel(bank);
  addControl(new PresetListHeaderArrow(isLast ? " " : "\uE301",
                                       Rect(pos.getWidth() - arrowWidth, -1, arrowWidth, arrowHeight), 1));
}
