#include "PresetBank.h"
#include "EditBuffer.h"
#include "Application.h"
#include "device-settings/AutoLoadSelectedPreset.h"
#include "device-settings/Settings.h"
#include "Preset.h"
#include "xml/Writer.h"
#include "libundo/undo/Scope.h"
#include "libundo/undo/SwapData.h"
#include <sstream>
#include <xml/Attribute.h>
#include <xml/FileInStream.h>
#include <xml/VersionAttribute.h>
#include <xml/CommitableFileOutStream.h>
#include <serialization/PresetBankMetadataSerializer.h>
#include <proxies/hwui/HWUI.h>
#include <tools/PerformanceTimer.h>
#include <libundo/undo/TrashTransaction.h>
#include <tools/FileSystem.h>
#include "http/UndoScope.h"
#include <proxies/hwui/panel-unit/boled/SplashLayout.h>
#include <device-info/DateTimeInfo.h>
#include <tools/TimeTools.h>

PresetBank::PresetBank(PresetManager *parent) :
    UpdateDocumentContributor(parent),
    m_signalBankChangedPending(false),
    m_attachment("", AttachmentDirection::none)
{
}

PresetBank::~PresetBank()
{
}

PresetManager *PresetBank::getParent()
{
  return static_cast<PresetManager *>(UpdateDocumentContributor::getParent());
}

const PresetManager *PresetBank::getParent() const
{
  return static_cast<const PresetManager *>(UpdateDocumentContributor::getParent());
}

void PresetBank::insertPreset(vector<tPresetPtr>::iterator it)
{
  tPresetPtr p = createPreset();
  insertPreset(it, p);
}

void PresetBank::insertPreset(vector<tPresetPtr>::iterator it, tPresetPtr p)
{
  m_presets.insert(it, p);
}

PresetBank::tPresetPtr PresetBank::createPreset()
{
  return Preset::createPreset(this);
}

void PresetBank::loadSync(shared_ptr<UNDO::Transaction> transaction, RefPtr<Gio::File> bankFolder, int numBank, int numBanks)
{
  auto lastChangeOnFile = loadMetadata(transaction, bankFolder);

  SplashLayout::addStatus(UNDO::StringTools::buildString("Loading Bank ", numBank, " of ", numBanks, ": \"", getName(false), "\""));

  loadPresets(transaction, bankFolder);
  deleteOldPresetFiles(bankFolder);

  m_metadataLastSavedForUpdateID = getUpdateIDOfLastChange();
  m_lastChangedTimestamp = lastChangeOnFile;

}

void PresetBank::deleteOldPresetFiles(RefPtr<Gio::File> bankFolder)
{
  RefPtr<Gio::FileEnumerator> enumerator = bankFolder->enumerate_children();

  while(auto file = enumerator->next_file())
  {
    if(file->get_file_type() == Gio::FILE_TYPE_REGULAR)
    {
      auto uuid = file->get_name();

      if(FileSystem::isNameAUUID(uuid))
      {
        if(!getPreset(uuid))
        {
          bankFolder->get_child(uuid)->remove();
        }
      }
    }
  }
}

SaveResult PresetBank::save(RefPtr<Gio::File> bankFolder)
{
  try
  {
    if(saveMetadata(bankFolder) == SaveResult::Finished)
      return SaveResult::Again;

    if(savePresets(bankFolder) == SaveResult::Again)
      return SaveResult::Again;

    return SaveResult::Nothing;
  }
  catch(...)
  {
    DebugLevel::error("Could not save bank", getName(true));
  }
  return SaveResult::Nothing;
}

uint64_t PresetBank::loadMetadata(UNDO::Scope::tTransactionPtr transaction, RefPtr<Gio::File> bankFolder)
{
  auto lastChangeOnFile = Serializer::read<PresetBankMetadataSerializer>(transaction, bankFolder, ".metadata", shared_from_this());
  m_metadataLastSavedForUpdateID = getUpdateIDOfLastChange();
  return lastChangeOnFile;
}

SaveResult PresetBank::saveMetadata(RefPtr<Gio::File> bankFolder)
{
  if(m_metadataLastSavedForUpdateID != getUpdateIDOfLastChange())
  {
    PresetBankMetadataSerializer serializer(shared_from_this());
    serializer.write(bankFolder, ".metadata");
    m_metadataLastSavedForUpdateID = getUpdateIDOfLastChange();
    return SaveResult::Finished;
  }
  return SaveResult::Nothing;
}

void PresetBank::loadPresets(UNDO::Scope::tTransactionPtr transaction, RefPtr<Gio::File> bankFolder)
{
  list<std::shared_ptr<Preset>> toDelete;

  for(auto p : m_presets)
  {
    try
    {
      p->load(transaction, bankFolder);
    }
    catch(...)
    {
      DebugLevel::error("could not load preset", p->getUuid());
      toDelete.push_back(p);
    }
  }

  for(auto b : toDelete)
  {
    undoableDeletePreset(transaction, b->getUuid());
  }

  m_presetsLastSavedForUpdateID = getUpdateIDOfLastChange();
}

SaveResult PresetBank::savePresets(RefPtr<Gio::File> bankFolder)
{
  if(m_presetsLastSavedForUpdateID != getUpdateIDOfLastChange())
  {
    for(auto p : m_presets)
    {
      if(p->save(bankFolder))
      {
        return SaveResult::Again;
      }
    }
    m_presetsLastSavedForUpdateID = getUpdateIDOfLastChange();
  }
  return SaveResult::Nothing;
}

void PresetBank::undoableDeletePreset(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &uuid)
{
  size_t pos = getPresetPosition(uuid);

  if(pos != Glib::ustring::npos)
  {
    tPresetPtr swappedPreset;
    auto swap = UNDO::createSwapData(swappedPreset);

    bool wasSelectedPreset = m_presets[pos]->getUuid() == m_selectedPresetUUID;

    transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
    {
      swap->swapWith (m_presets[pos]);
      m_presets.erase (m_presets.begin () + pos);
      onChange();
    }, [ = ] (UNDO::Command::State) mutable
    {
      insertPreset (m_presets.begin() + pos);
      swap->swapWith (m_presets[pos]);
      m_presets[pos]->onRestore();
      onChange();
    });

    if(wasSelectedPreset)
    {
      if(pos > 0)
        getPreset(pos - 1)->undoableSelect(transaction);
      else if(getNumPresets() > 0)
        getPreset(0)->undoableSelect(transaction);
    }
  }
}

void PresetBank::undoableSetName(const Glib::ustring &name)
{
  UNDO::Scope::tTransactionScopePtr scope = getUndoScope().startTransaction("Rename bank '%0' to '%1'", getName(true), name);
  undoableSetName(scope->getTransaction(), name);
}

void PresetBank::undoableSetName(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &name)
{
  auto swapData = UNDO::createSwapData(name);

  transaction->addSimpleCommand([ = ] (UNDO::Command::State)
  {
    swapData->swapWith (m_name);
    onChange();
  });

  getParent()->undoableSelectBank(transaction, getUuid());
  getParent()->getEditBuffer()->undoableUpdateLoadedPresetInfo(transaction);
}

void PresetBank::undoableSelect(UNDO::Scope::tTransactionPtr transaction)
{
  getParent()->undoableSelectBank(transaction, getUuid());
}

void PresetBank::undoableEnsurePresetSelection(UNDO::Scope::tTransactionPtr transaction)
{
  auto selPresetUUID = getSelectedPreset();

  if(!getPreset(selPresetUUID) && getNumPresets() > 0)
  {
    undoableSelectPreset(transaction, getPreset(0)->getUuid());
  }
}

void PresetBank::undoableSetPosition(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &x, const Glib::ustring &y)
{
  auto swapData = UNDO::createSwapData(x, y);

  transaction->addSimpleCommand([ = ] (UNDO::Command::State)
  {
    swapData->swapWith<0> (m_X);
    swapData->swapWith<1> (m_Y);
    onChange();
  });
}

void PresetBank::undoableMovePosition(UNDO::Scope::tTransactionPtr transaction, double x, double y)
{
  x += stod(m_X);
  y += stod(m_Y);
  undoableSetPosition(transaction, to_string(x), to_string(y));
}

void PresetBank::undoableMovePreset(size_t from, size_t to)
{
  auto swapData = UNDO::createSwapData(from, to);

  UNDO::Scope::tTransactionScopePtr scope = getUndoScope().startTransaction("Move Preset '%0' from %1 to %2",
      generateHumanReadablePresetName(from), from + 1, to + 1);

  UNDO::Scope::tTransactionPtr transaction = scope->getTransaction();

  transaction->addSimpleCommand([ = ] (UNDO::Command::State)
  {
    m_presets[swapData->get<1>()] = getPreset (swapData->get<0>());
    m_presets[swapData->get<0>()] = createPreset();
    onChange();
    swapData->swapWith (swapData->get<1>());
  });
}

shared_ptr<Preset> PresetBank::undoableExpropriatePreset(UNDO::Scope::tTransactionPtr transaction, const Uuid &uuid)
{
  auto preset = getPreset(uuid);
  auto pos = getPresetPosition(uuid);

  if(pos != Glib::ustring::npos)
  {
    transaction->addSimpleCommand([ = ] (UNDO::Command::State)
    {
      m_presets.erase(m_presets.begin() + pos);
      onChange();
    }, [=](UNDO::Command::State)
    {
      m_presets.insert(m_presets.begin() + pos, preset);
      preset->adopt(this);
      onChange();
    });
  }
  return preset;
}

void PresetBank::undoableAdoptPreset(UNDO::Scope::tTransactionPtr transaction, size_t pos, shared_ptr<Preset> preset)
{
  transaction->addSimpleCommand([ = ] (UNDO::Command::State)
  {
    m_presets.insert(m_presets.begin() + pos, preset);
    preset->adopt(this);
    onChange();
  }, [=](UNDO::Command::State)
  {
    m_presets.erase(m_presets.begin() + pos);
    onChange();
  });
}

void PresetBank::undoableStorePreset(size_t pos, shared_ptr<EditBuffer> editBuffer)
{
  UNDO::Scope::tTransactionScopePtr scope = getUndoScope().startTransaction("Store EditBuffer at position %0", pos + 1);
  UNDO::Scope::tTransactionPtr transaction = scope->getTransaction();
  undoableOverwritePreset(transaction, pos, editBuffer);
}

void PresetBank::undoableOverwritePreset(UNDO::Scope::tTransactionPtr transaction, size_t pos, tPresetPtr preset)
{
  auto p = getPreset(pos);
  undoableOverwritePreset(transaction, p.get(), preset.get());
}

void PresetBank::undoableOverwritePreset(UNDO::Scope::tTransactionPtr transaction, Preset *presetToOverwrite, Preset *src)
{
  presetToOverwrite->copyFrom(transaction, src, true);
  presetToOverwrite->setAutoGeneratedAttributes(transaction);

  auto eb = getParent()->getEditBuffer();

  if(src == eb.get())
  {
    eb->undoableSetLoadedPresetInfo(transaction, presetToOverwrite);
    eb->resetModifiedIndicator(transaction);
  }
}

void PresetBank::undoableInsertPreset(UNDO::Scope::tTransactionPtr transaction, int pos)
{
  tPresetPtr cleanPreset(createPreset());
  auto swapData = UNDO::createSwapData(cleanPreset);

  transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
  {
    insertPreset (m_presets.begin() + pos, swapData->get<0>());
    onChange();
  }, [ = ] (UNDO::Command::State) mutable
  {
    m_presets.erase (m_presets.begin () + pos);
    onChange();
  });
}

void PresetBank::undoableAppendPreset(UNDO::Scope::tTransactionPtr transaction, const Uuid &uuid)
{
  auto pos = getNumPresets();
  undoableInsertPreset(transaction, pos);
  getPreset(pos)->undoableSetUuid(transaction, uuid);
}

void PresetBank::undoableSelectPreset(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &uuid)
{
  if(m_selectedPresetUUID != uuid)
  {
    auto swapData = UNDO::createSwapData(uuid);

    transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
    {
      auto suppressUpdatingLastChangedTimestamp = m_lastChangedTimestamp;
      swapData->swapWith (m_selectedPresetUUID);
      onChange();
      m_lastChangedTimestamp = suppressUpdatingLastChangedTimestamp;
    });

    if(Application::get().getSettings()->getSetting<AutoLoadSelectedPreset>()->get())
    {
      getParent()->doAutoLoadSelectedPreset();
    }
  }
}

Glib::ustring PresetBank::generateHumanReadablePresetName(size_t pos) const
{
  Glib::ustring ret("'");
  ret += getPreset(pos)->getName();
  ret += "'";
  return ret;
}

void PresetBank::setName(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &name)
{
  auto swapData = UNDO::createSwapData(name);

  transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
  {
    swapData->swapWith (m_name);
    onChange();
  });
}

void PresetBank::copyFrom(UNDO::Scope::tTransactionPtr transaction, shared_ptr<PresetBank> other, bool ignoreUUIDs)
{
  setName(transaction, other->getName(false));
  undoableSetPosition(transaction, other->m_X, other->m_Y);

  size_t numPresets = other->getNumPresets();

  while(getNumPresets() > numPresets)
    undoableDeletePreset(transaction, getPreset(0)->getUuid());

  while(getNumPresets() < numPresets)
    undoableInsertPreset(transaction, 0);

  for(size_t i = 0; i < numPresets; i++)
  {
    getPreset(i)->copyFrom(transaction, other->getPreset(i).get(), ignoreUUIDs);
  }

  if(!ignoreUUIDs)
    m_uuid = other->getUuid();

  auto srcSelectedPreset = other->getSelectedPreset();
  auto p = other->getPresetPosition(srcSelectedPreset);

  if(p < getNumPresets())
  {
    if(auto tgtPreset = getPreset(p))
    {
      undoableSelectPreset(transaction, tgtPreset->getUuid());
    }
  }

  AttributesOwner::copyFrom(transaction, other.get());
}

void PresetBank::setUuid(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &id)
{
  Uuid uuid(id);
  auto swapData = UNDO::createSwapData(uuid);

  transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
  {
    swapData->swapWith (m_uuid);
    onChange();
  });
}

void PresetBank::setX(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &x)
{
  auto swapData = UNDO::createSwapData(x);

  transaction->addSimpleCommand([ = ] (UNDO::Command::State)
  {
    swapData->swapWith (m_X);
    onChange();
  });
}

void PresetBank::setY(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &y)
{
  auto swapData = UNDO::createSwapData(y);

  transaction->addSimpleCommand([ = ] (UNDO::Command::State)
  {
    swapData->swapWith (m_Y);
    onChange();
  });
}

const Glib::ustring &PresetBank::getX() const
{
  return m_X;
}

const Glib::ustring &PresetBank::getY() const
{
  return m_Y;
}

Glib::ustring PresetBank::getName(bool withFallback) const
{
  if(withFallback && m_name.empty())
    return "<untitled preset bank>";

  return m_name;
}

const Uuid &PresetBank::getUuid() const
{
  return m_uuid;
}

size_t PresetBank::getNumPresets() const
{
  return m_presets.size();
}

PresetBank::tPresetPtr PresetBank::getPreset(size_t pos)
{
  if(pos < getNumPresets())
    return m_presets[pos];

  return nullptr;
}

const PresetBank::tPresetPtr PresetBank::getPreset(size_t pos) const
{
  if(pos < getNumPresets())
    return m_presets[pos];

  return nullptr;
}

const PresetBank::tPresetPtr PresetBank::getPreset(const Uuid &uuid) const
{
  for(auto & it : m_presets)
    if(it->getUuid() == uuid)
      return it;

  return nullptr;
}

PresetBank::tPresetPtr PresetBank::getPreset(const Uuid &uuid)
{
  for(auto & it : m_presets)
    if(it->getUuid() == uuid)
      return it;

  return nullptr;
}

const PresetBank::tPresetPtr PresetBank::findPresetByName(const Glib::ustring &name) const
{
  for(auto & it : m_presets)
    if(it->getName() == name)
      return it;

  return nullptr;
}

sigc::connection PresetBank::onBankChanged(sigc::slot<void> slot)
{
  return m_signalBankChanged.connectAndInit(slot);
}

UpdateDocumentContributor::tUpdateID PresetBank::onChange(uint64_t flags)
{
  m_lastChangedTimestamp = TimeTools::getAdjustedTimestamp();

  if(getParent())
  {
    signalBankChanged();
    return UpdateDocumentContributor::onChange(flags);
  }
  return 0;
}

void PresetBank::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
  bool changed = knownRevision < getUpdateIDOfLastChange();

  writer.writeTag("preset-bank", Attribute("uuid", (Glib::ustring) m_uuid), Attribute("name", m_name), Attribute("x", m_X),
      Attribute("y", m_Y), Attribute("selected-preset", m_selectedPresetUUID),
      Attribute("order-number", getParent()->calcOrderNumber(this)), Attribute("changed", changed), [&]()
      {
        if (changed)
        {
          AttributesOwner::writeAttributes(writer);

          for (const tPresetPtr p : m_presets)
          {
            p->writeDocument (writer, knownRevision);
          }

          writer.writeTextElement ("date-of-last-change", TimeTools::getDisplayStringFromStamp(m_lastChangedTimestamp));
          writer.writeTextElement ("attached-to", m_attachment.uuid);
          writer.writeTextElement ("attached-direction", directionEnumToString(m_attachment.direction));
          writer.writeTextElement("state", calcStateString());

        }
      });
}

void PresetBank::searchPresets(Writer &writer, const SearchQuery &query) const
{
  writer.writeTag("preset-bank", Attribute("uuid", (Glib::ustring) m_uuid), Attribute("name", m_name),
      Attribute("order-number", getParent()->calcOrderNumber(this)), [&]()
      {
        for (const tPresetPtr p : m_presets)
        {
          if(p->matchesQuery(query))
          {
            p->writeDocument (writer, 0);
          }
        }
      });
}

void PresetBank::signalBankChanged()
{
  if(!m_signalBankChangedPending.exchange(true))
    MainContext::get_default()->signal_idle().connect(sigc::mem_fun(this, &PresetBank::sendBankChangedOnIdle));
}

bool PresetBank::sendBankChangedOnIdle()
{
  m_signalBankChangedPending.store(false);
  m_signalBankChanged.send();
  return false;
}

size_t PresetBank::getPresetPosition(const Uuid &uuid) const
{
  size_t numPresets = m_presets.size();

  for(size_t i = 0; i < numPresets; i++)
  {
    if(m_presets[i] && m_presets[i]->getUuid() == uuid)
      return i;
  }

  return Glib::ustring::npos;
}

bool PresetBank::containsPreset(const Uuid &uuid) const
{
  return getPresetPosition(uuid) != Glib::ustring::npos;
}

Glib::ustring PresetBank::getSelectedPreset() const
{
  return m_selectedPresetUUID;
}

bool PresetBank::setSelectedPreset(Glib::ustring uuid)
{
  for(tPresetPtr presets : m_presets)
  {
    if(presets->getUuid() == uuid)
    {
      m_selectedPresetUUID = uuid;
      return true;
    }
  }
  return false;
}

void PresetBank::undoableIncPresetSelection(int inc, ButtonModifiers modifiers)
{
  size_t pos = getPresetPosition(getSelectedPreset());

  if(pos != Glib::ustring::npos && getNumPresets() > 0)
  {
    int newPos = 0;

    if(modifiers[ButtonModifier::SHIFT])
    {
      if(inc < 0)
      {
        newPos = 0;
      }
      else
      {
        newPos = getNumPresets() - 1;
      }
    }
    else
    {
      inc = std::min<int>((int) getNumPresets() - pos - 1, inc);
      inc = std::max<int>(-pos, inc);
      newPos = pos + inc;
    }

    if(auto p = getPreset(newPos))
    {
      UNDO::Scope::tTransactionScopePtr scope;

      bool autoLoad = Application::get().getSettings()->getSetting<AutoLoadSelectedPreset>()->get();

      if(autoLoad)
        scope = getUndoScope().startTransaction(p->getUndoTransactionTitle("Load"));
      else
        scope = getUndoScope().startContinuousTransaction(getParent(), chrono::hours(1), p->getUndoTransactionTitle("Select"));

      undoableSelectPreset(scope->getTransaction(), p->getUuid());
    }
  }
}

int PresetBank::getHighestIncrementForBaseName(const Glib::ustring &baseName) const
{
  static auto regex = Glib::Regex::create("^(.*)\\s([0-9]+)$");

  bool hadMatch = false;

  int h = 0;

  for(auto p : m_presets)
  {
    ustring name = p->getName();

    if(name == baseName)
    {
      h = std::max(h, 1);
      hadMatch = true;
    }
    else if(name.find(baseName) == 0)
    {
      Glib::MatchInfo matchInfo;

      if(regex->match(name, matchInfo) && matchInfo.get_match_count() > 1)
      {
        auto presetsBaseName = matchInfo.fetch(1);

        if(presetsBaseName == baseName)
        {
          auto number = matchInfo.fetch(2);
          int newNumber = stoi(number);
          h = std::max(h, newNumber);
          hadMatch = true;
        }
      }
    }
  }

  if(hadMatch)
    return std::max(1, h);

  return 0;
}

auto PresetBank::getPresets() const -> const vector<tPresetPtr>&
{
  return m_presets;
}

void PresetBank::undoableSort(UNDO::Scope::tTransactionPtr transaction, bool asc)
{
  auto sorted = m_presets;

  std::sort(sorted.begin(), sorted.end(), [asc] (tPresetPtr a, tPresetPtr b)
  {
    if (asc)
    return a->getName() < b->getName();

    return a->getName() > b->getName();
  });

  auto swap = UNDO::createSwapData(sorted);

  transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
  {
    swap->swapWith (m_presets);
    onChange();
  });
}

uint64_t PresetBank::getLastChangedTimestamp() const
{
  return m_lastChangedTimestamp;
}

const Glib::ustring PresetBank::calcStateString() const
{
  auto timestampWithTolerance = m_lastChangedTimestamp - 5;

  std::string lastExportTimeIso = getAttribute("Date of Export File", "-1");
  std::string lastModTimeIso = TimeTools::getIsoTime(timestampWithTolerance);
  std::string lastImportTime = getAttribute("Date of Import File", "-1");

  if(lastImportTime != "-1" && lastModTimeIso <= lastImportTime)
  {
    return "Unchanged since Import";
  }
  else if(lastModTimeIso.compare(lastExportTimeIso) < 0 || lastModTimeIso == lastExportTimeIso)
  {
    return "Saved by Export";
  }
  else
  {
    return "Not Saved By Export";
  }
}

void PresetBank::undoableAssignDefaultPosition(shared_ptr<UNDO::Transaction> transaction)
{
  auto xy = calcDefaultPosition();
  undoableMovePosition(transaction, xy.first, xy.second);
}

void PresetBank::assignDefaultPosition()
{
  auto xy = calcDefaultPosition();
  m_X = to_string(xy.first);
  m_Y = to_string(xy.second);
}

std::pair<double, double> PresetBank::calcDefaultPosition() const
{
  const auto &banks = getParent()->getBanks();

  if(!banks.empty())
  {
    auto rightMost = banks.front();

    for(auto b : banks)
    {
      if(b.get() != this)
      {
        auto x = stod(b->getX());
        auto currentX = stod(rightMost->getX());

        if(x > currentX)
          rightMost = b;
      }
    }

    return std::make_pair(stod(rightMost->getX()) + 300, stod(rightMost->getY()));
  }

  return std::make_pair(0.0, 0.0);
}

void PresetBank::undoableAttachBank(UNDO::Scope::tTransactionPtr transaction, Glib::ustring masterUuid, AttachmentDirection dir)
{
  auto swapData = UNDO::createSwapData(Attachment(masterUuid, dir));
  transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
  {
    swapData->swapWith (m_attachment);
    onChange();
  });

  DebugLevel::warning("undoableAttach after Swap: ", m_attachment.uuid, "dir", directionEnumToString(m_attachment.direction));

}
void PresetBank::undoableDetachBank(UNDO::Scope::tTransactionPtr transaction)
{
  undoableAttachBank(transaction, "", AttachmentDirection::none);
}

const PresetBank::Attachment &PresetBank::getAttached() const
{
  return m_attachment;
}

void PresetBank::resetAttached(UNDO::Scope::tTransactionPtr transaction)
{
  setAttachedTo(transaction, "");
  setAttachedDirection(transaction, AttachmentDirection::none);
}

void PresetBank::setAttachedTo(UNDO::Scope::tTransactionPtr transaction, Glib::ustring attachedTo)
{
  auto swapData = UNDO::createSwapData(attachedTo);

  transaction->addSimpleCommand([=] (UNDO::Command::State) mutable
  {
    swapData->swapWith(m_attachment.uuid);
    onChange();
  });
}
void PresetBank::setAttachedDirection(UNDO::Scope::tTransactionPtr transaction, AttachmentDirection direction)
{
  auto swapData = UNDO::createSwapData(direction);

  transaction->addSimpleCommand([=] (UNDO::Command::State) mutable
  {
    swapData->swapWith(m_attachment.direction);
    onChange();
  });
}

const Glib::ustring PresetBank::directionEnumToString(AttachmentDirection direction) const
{
  static_assert(AttachmentDirection::none == 0, "Nicht den Enum ändern ohne diese Funktion und Java Seite zu ändern!");
  static_assert(AttachmentDirection::top == 1, "Nicht den Enum ändern ohne diese Funktion und Java Seite zu ändern!");
  static_assert(AttachmentDirection::left == 2, "Nicht den Enum ändern ohne diese Funktion und Java Seite zu ändern!");

  switch(direction)
  {

    case top:
      return "top";
    case left:
      return "left";
    case none:
    default:
      return "";
  }
}

void PresetBank::resolveCyclicAttachments(UNDO::Scope::tTransactionPtr transaction)
{
  std::vector<PresetBank*> banks;
  resolveCyclicAttachments(banks, transaction);
}

bool PresetBank::resolveCyclicAttachments(std::vector<PresetBank*> stackedBanks, UNDO::Scope::tTransactionPtr transaction)
{
  if(std::find(stackedBanks.begin(), stackedBanks.end(), this) != stackedBanks.end())
  {
    return false;
  }

  stackedBanks.push_back(this);

  auto master = Application::get().getPresetManager()->findBank(getAttached().uuid);

  if(master != nullptr && !master->resolveCyclicAttachments(stackedBanks, transaction))
  {
    resetAttached(transaction);
  }
  stackedBanks.pop_back();
  return true;
}

PresetBank *PresetBank::getClusterMaster()
{
  if(auto master = getParent()->findBank(getAttached().uuid))
    return master->getClusterMaster();

  return this;
}
