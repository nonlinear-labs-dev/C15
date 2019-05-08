#include <Application.h>
#include <presets/PresetBank.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/BankNameLabel.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListHeader.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListHeaderArrow.h>

PresetListHeader::PresetListHeader(const Rect& pos, bool showBankArrows)
    : super(pos)
    , m_showBankArrows(showBankArrows)
{
}

PresetListHeader::~PresetListHeader()
{
}

void PresetListHeader::setup(shared_ptr<PresetBank> bank)
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

void PresetListHeader::setupWithArrows(shared_ptr<PresetBank> bank)
{
  clear();

  auto pm = Application::get().getPresetManager();

  const Rect& pos = getPosition();
  const int arrowWidth = 14;

  auto order = pm->calcOrderNumber(bank.get());
  auto isFirst = order == 1;
  auto isLast = order == pm->getNumBanks();

  addControl(new PresetListHeaderArrow(isFirst ? " " : "e", Rect(0, 0, arrowWidth, 13)));
  addControl(new BankNameLabel(Rect(arrowWidth, 0, pos.getWidth() - 2 * arrowWidth, 13)))->updateLabel(bank);
  addControl(new PresetListHeaderArrow(isLast ? " " : "r", Rect(pos.getWidth() - arrowWidth, 0, arrowWidth, 13)));
}
