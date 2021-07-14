#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include "BankUseCases.h"
#include "presets/Bank.h"
#include "presets/PresetManager.h"
#include "presets/Preset.h"
#include "presets/EditBuffer.h"
#include <device-settings/Settings.h>
#include <device-settings/DirectLoadSetting.h>
#include <Application.h>
#include <tools/FileSystem.h>
#include <tools/TimeTools.h>
#include <xml/FileOutStream.h>
#include <xml/VersionAttribute.h>

BankUseCases::BankUseCases(Bank* bank)
    : m_bank { bank }
{
  nltools_assertAlways(m_bank != nullptr);
}

void BankUseCases::setBankComment(const Glib::ustring& comment)
{
  auto scope = m_bank->getUndoScope().startTransaction("Set Bank Comment");
  m_bank->setAttribute(scope->getTransaction(), "Comment", comment);
}

void BankUseCases::renameBank(const Glib::ustring& name)
{
  auto& undoScope = m_bank->getUndoScope();
  auto transactionScope = undoScope.startTransaction("Rename Bank '%0' to '%1'", m_bank->getName(true), name);
  auto transaction = transactionScope->getTransaction();
  m_bank->setName(transaction, name);

  if(auto pm = m_bank->getPresetManager())
  {
    pm->getEditBuffer()->undoableUpdateLoadedPresetInfo(transaction);
  }
}

void BankUseCases::stepPresetSelection(int inc)
{
  if(auto presetManager = m_bank->getPresetManager())
  {
    const bool directLoad = isDirectLoadActive();
    auto current = m_bank->getSelectedPreset();
    auto currentPos = m_bank->getPresetPosition(current);
    auto presetPosToSelect = std::max(0, std::min<int>(m_bank->getNumPresets() - 1, currentPos + inc));
    auto selectedPreset = presetManager->getSelectedPreset();

    if(auto presetToSelect = m_bank->getPresetAt(presetPosToSelect))
    {
      if(presetToSelect != selectedPreset)
      {
        Glib::ustring name {};
        if(directLoad)
          name = presetToSelect->buildUndoTransactionTitle("Select and Load Preset");
        else
          name = presetToSelect->buildUndoTransactionTitle("Select Preset");

        auto scope = m_bank->getUndoScope().startContinuousTransaction(m_bank, std::chrono::hours(1), name);
        m_bank->selectPreset(scope->getTransaction(), presetToSelect->getUuid());

        if(directLoad)
        {
          presetManager->getEditBuffer()->undoableLoad(scope->getTransaction(), presetToSelect, true);
        }
      }
    }
  }
}

void BankUseCases::dropPresets(const std::string& csv)
{
  if(auto pm = m_bank->getPresetManager())
  {
    auto scope = m_bank->getUndoScope().startTransaction("Drop Presets on Bank");
    auto transaction = scope->getTransaction();

    std::vector<std::string> strs;
    boost::split(strs, csv, boost::is_any_of(","));

    for(const auto& uuid : strs)
    {
      if(auto src = pm->findPreset(Uuid { uuid }))
      {
        m_bank->appendPreset(transaction, std::make_unique<Preset>(m_bank, *src, true));

        if(m_bank == src->getParent())
          m_bank->deletePreset(transaction, Uuid { uuid });
      }
    }
  }
}

void BankUseCases::dropBank(const Bank* source)
{
  if(auto pm = m_bank->getPresetManager())
  {
    if(m_bank != source && source)
    {
      auto insertPos = m_bank->getNumPresets();
      auto scope = pm->getUndoScope().startTransaction("Drop bank '%0' into bank '%1'", source->getName(true),
                                                       m_bank->getName(true));
      auto transaction = scope->getTransaction();
      size_t i = 0;

      source->forEachPreset([&](auto p) {
        m_bank->insertPreset(transaction, insertPos + i, std::make_unique<Preset>(m_bank, *p, true));
        i++;
      });
    }
  }
}

void BankUseCases::dropBankOnPreset(const Bank* sourceBank, const Uuid& presetAnchor)
{
  if(m_bank != sourceBank)
  {
    auto srcBankName = sourceBank->getName(true);
    auto tgtBankName = m_bank->getName(true);
    auto scope = m_bank->getUndoScope().startTransaction("Drop bank '%0' into bank '%1'", srcBankName, tgtBankName);
    auto transaction = scope->getTransaction();

    size_t insertPos = m_bank->getPresetPosition(presetAnchor) + 1;
    size_t numPresets = sourceBank->getNumPresets();

    for(size_t i = 0; i < numPresets; i++)
    {
      auto p = std::make_unique<Preset>(m_bank, *sourceBank->getPresetAt(i), true);
      m_bank->insertPreset(transaction, insertPos + i, std::move(p));
    }

    m_bank->deletePreset(scope->getTransaction(), presetAnchor);
  }
}

void BankUseCases::moveBank(const std::string& x, const std::string& y)
{
  if(m_bank->getX() != x || m_bank->getY() != y)
  {
    auto scope = m_bank->getUndoScope().startTransaction("Move preset bank '%0'", m_bank->getName(true));
    auto transaction = scope->getTransaction();
    m_bank->setX(transaction, x);
    m_bank->setY(transaction, y);
  }
}

void BankUseCases::deletePreset(const Preset* p)
{
  if(p)
  {
    deletePreset(p->getUuid());
  }
}

void BankUseCases::deletePreset(const Uuid& uuid)
{
  if(auto preset = m_bank->findPreset(uuid))
  {
    auto scope = m_bank->getUndoScope().startTransaction("Delete preset '%0'", preset->getName());
    m_bank->deletePreset(scope->getTransaction(), preset->getUuid());

    if(auto pm = m_bank->getPresetManager())
    {
      pm->getEditBuffer()->undoableUpdateLoadedPresetInfo(scope->getTransaction());
    }
  }
}

void BankUseCases::selectPreset(int pos)
{
  if(pos < m_bank->getNumPresets())
  {
    if(auto presetToSelect = m_bank->getPresetAt(pos))
    {
      if(m_bank->getPresetManager()->getSelectedPreset() != presetToSelect)
      {
        const auto directLoad = isDirectLoadActive();
        Glib::ustring name {};
        if(directLoad)
          name = presetToSelect->buildUndoTransactionTitle("Select and Load Preset");
        else
          name = presetToSelect->buildUndoTransactionTitle("Select Preset");

        auto scope = m_bank->getUndoScope().startContinuousTransaction(m_bank, std::chrono::hours(1), name);
        m_bank->selectPreset(scope->getTransaction(), pos);

        if(isDirectLoadActive())
        {
          m_bank->getPresetManager()->getEditBuffer()->undoableLoad(scope->getTransaction(), presetToSelect, true);
        }
      }
    }
  }
}

void BankUseCases::setAttribute(const Glib::ustring& key, const Glib::ustring& value)
{
  auto scope = m_bank->getUndoScope().startTransaction("Set Bank attribute");
  auto transaction = scope->getTransaction();
  m_bank->setAttribute(transaction, key, value);
  m_bank->updateLastModifiedTimestamp(transaction);
}

bool BankUseCases::isDirectLoadActive() const
{
  return Application::get().getSettings()->getSetting<DirectLoadSetting>()->get();
}

void BankUseCases::setCollapsed(bool b)
{
  std::string name = b ? "Collapse" : "Maximize";
  auto scope = m_bank->getUndoScope().startTransaction(name + " Bank");
  auto transaction = scope->getTransaction();
  m_bank->setAttribute(transaction, "collapsed", b ? "true" : "false");
  m_bank->updateLastModifiedTimestamp(transaction);
}

void BankUseCases::exportBankToFile(const std::string& outFile)
{
  GenericScopeGuard syncAfterAllFileOperation([] {}, FileSystem::syncAll);
  auto scope = UNDO::Scope::startTrashTransaction();
  m_bank->setAttribute(scope->getTransaction(), "Date of Export File", TimeTools::getAdjustedIso());
  m_bank->setAttribute(scope->getTransaction(), "Name of Export File", outFile);
  PresetBankSerializer serializer(m_bank, false);
  FileOutStream stream(outFile, false);
  XmlWriter writer(stream);
  serializer.write(writer, VersionAttribute::get());
}
