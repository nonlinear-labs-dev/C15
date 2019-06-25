#include <Application.h>
#include <presets/Preset.h>
#include <presets/Bank.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/LeftAlignedLabel.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MultiLineLabel.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/BankInfoContent.h>
#include <iomanip>
#include <sstream>
#include <device-info/DateTimeInfo.h>
#include <tools/TimeTools.h>

namespace DETAIL
{
  class BankComment : public MultiLineLabel
  {
   public:
    BankComment(Rect pos)
        : MultiLineLabel("---")
    {
      setPosition(pos);
    }

    Oleds::tFont getFont()
    {
      return Oleds::get().getFont("Emphase_8_TXT_Regular", 8);
    }
  };
}

BankInfoContent::BankInfoContent()
{
  addInfoField("name", "Name", new MultiLineContent());
  addInfoField("size", "Size");
  addInfoField("comment", "Comment", new MultiLineContent());
  addInfoField("state", "State");
  addInfoField("dateofchange", "Last Change");
  addInfoField("importdate", "Import Date");
  addInfoField("importfile", "Import File");
  addInfoField("exportdate", "Export Date");
  addInfoField("exportfile", "Export File");

  Application::get().getPresetManager()->onBankSelection(mem_fun(this, &BankInfoContent::onBankSelectionChanged));
  fillFromBank(Application::get().getPresetManager()->getSelectedBank());
}

BankInfoContent::~BankInfoContent()
{
}

void BankInfoContent::onBankSelectionChanged(const Uuid &selectedBank)
{
  m_bankConnection.disconnect();

  if(auto bank = Application::get().getPresetManager()->getSelectedBank())
    m_bankConnection = bank->onBankChanged(sigc::bind(mem_fun(this, &BankInfoContent::onBankChanged), bank));
}

void BankInfoContent::onBankChanged(Bank *bank)
{
  if(bank)
  {
    if(fillFromBank(bank))
    {
      fixLayout();
    }
  }
  else if(fillDefaults())
  {
    fixLayout();
  }
}

bool BankInfoContent::fillFromBank(Bank *bank)
{
  infoFields["name"]->setInfo(bank->getName(true), FrameBuffer::Colors::C128);
  infoFields["size"]->setInfo(to_string(bank->getNumPresets()));
  infoFields["comment"]->setInfo(bank->getAttribute("Comment", "---"), FrameBuffer::Colors::C128);
  infoFields["state"]->setInfo(bank->calcStateString());
  infoFields["dateofchange"]->setInfo(TimeTools::getDisplayStringFromStamp(bank->getLastChangedTimestamp()));
  infoFields["importdate"]->setInfo(
      TimeTools::getDisplayStringFromIso(bank->getAttribute("Date of Import File", "---")));
  infoFields["importfile"]->setInfo(bank->getAttribute("Name of Import File", "---"));
  infoFields["exportdate"]->setInfo(
      TimeTools::getDisplayStringFromIso(bank->getAttribute("Date of Export File", "---")));
  infoFields["exportfile"]->setInfo(bank->getAttribute("Name of Export File", "---"));
  return true;
}

bool BankInfoContent::fillDefaults()
{
  infoFields["name"]->setInfo("---", FrameBuffer::Colors::C128);
  infoFields["comment"]->setInfo("---", FrameBuffer::Colors::C128);
  infoFields["size"]->setInfo("---");
  infoFields["state"]->setInfo("Not saved by Export!");
  infoFields["dateofchange"]->setInfo("---");
  infoFields["exportdate"]->setInfo("---");
  infoFields["importdate"]->setInfo("---");
  infoFields["importfile"]->setInfo("---");
  infoFields["exportfile"]->setInfo("---");
  return true;
}

void BankInfoContent::setPosition(const Rect &rect)
{
  super::setPosition(rect);
  fixLayout();
}

const Rect &BankInfoContent::getPosition() const
{
  return super::getPosition();
}

void BankInfoContent::setDirty()
{
  super::setDirty();
  notifyDirty(true);
}

void BankInfoContent::fixLayout()
{
  int y = 0;

  for(auto infoKey :
      { "name", "comment", "size", "state", "dateofchange", "importdate", "importfile", "exportdate", "exportfile" })
  {
    y = infoFields[infoKey]->format(y);
  }

  Rect r = getPosition();
  r.setHeight(y);
  super::setPosition(r);
}
