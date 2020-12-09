#include "Bank.h"
#include <presets/Preset.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <serialization/PresetBankMetadataSerializer.h>
#include <tools/FileSystem.h>
#include <tools/TimeTools.h>
#include <device-settings/DebugLevel.h>
#include <nltools/Assert.h>
#include <glibmm/regex.h>
#include <giomm/file.h>
#include <Application.h>
#include <device-settings/Settings.h>
#include <proxies/hwui/HWUI.h>

std::string to_string(Bank::AttachmentDirection dir);

inline EditBuffer *getEditBuffer()
{
  return Application::get().getPresetManager()->getEditBuffer();
}

Bank::Bank(UpdateDocumentContributor *parent)
    : super(parent)
    , m_attachedToBankWithUuid(Uuid::none())
    , m_name("<Untitled Bank>")
    , m_presets(*this, std::bind(&Bank::clonePreset, this, std::placeholders::_1))
{
}

Bank::Bank(UpdateDocumentContributor *parent, const Bank &other, bool ignoreUuids)
    : Bank(parent)
{
  if(!ignoreUuids)
  {
    m_uuid = other.m_uuid;
    m_attachedToBankWithUuid = other.m_attachedToBankWithUuid;
  }

  {
    auto scope = UNDO::Scope::startTrashTransaction();
    AttributesOwner::copyFrom(scope->getTransaction(), &other);
  }

  m_name = other.m_name;
  m_serializationDate = other.m_serializationDate;
  m_x = other.m_x;
  m_y = other.m_y;
  m_attachDirection = other.m_attachDirection;
  m_presets = other.m_presets;
}

Bank::~Bank()
{
  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
}

SaveResult Bank::save(Glib::RefPtr<Gio::File> bankFolder)
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

SaveResult Bank::saveMetadata(Glib::RefPtr<Gio::File> bankFolder)
{
  if(m_metadataLastSavedForUpdateID != getUpdateIDOfLastChange())
  {
    PresetBankMetadataSerializer serializer(this);
    serializer.write(bankFolder, ".metadata");
    m_metadataLastSavedForUpdateID = getUpdateIDOfLastChange();
    return SaveResult::Finished;
  }
  return SaveResult::Nothing;
}

SaveResult Bank::savePresets(Glib::RefPtr<Gio::File> bankFolder)
{
  if(m_presetsLastSavedForUpdateID != getUpdateIDOfLastChange())
  {
    for(auto &p : m_presets.getElements())
      if(p->save(bankFolder))
        return SaveResult::Again;

    m_presetsLastSavedForUpdateID = getUpdateIDOfLastChange();
  }
  return SaveResult::Nothing;
}

Preset *Bank::clonePreset(const Preset *p)
{
  return new Preset(this, *p, true);
}

void Bank::load(UNDO::Transaction *transaction, Glib::RefPtr<Gio::File> bankFolder, int numBank, int numBanks)
{
  auto lastChangedDateOfFile = loadMetadata(transaction, bankFolder);
  loadPresets(transaction, bankFolder);
  deleteOldPresetFiles(bankFolder);

  m_lastChangedTimestamp = lastChangedDateOfFile;
  // to be overwritten by PresetBankMetadataSerializer addPostfixCommand, if timestamp exists in file
}

void Bank::deleteOldPresetFiles(Glib::RefPtr<Gio::File> bankFolder)
{
  if(bankFolder->query_exists())
  {
    Glib::RefPtr<Gio::FileEnumerator> enumerator = bankFolder->enumerate_children();

    while(auto file = enumerator->next_file())
    {
      if(file->get_file_type() == Gio::FILE_TYPE_REGULAR)
      {
        auto fileName = file->get_name();
        auto withoutExtension = fileName.substr(0, fileName.find("."));

        if(FileSystem::isNameAUUID(withoutExtension))
        {
          if(!findPreset(withoutExtension))
          {
            if(auto presetFile = bankFolder->get_child(fileName))
              presetFile->remove();
          }
        }
      }
    }
  }
}

uint64_t Bank::loadMetadata(UNDO::Transaction *transaction, Glib::RefPtr<Gio::File> bankFolder)
{
  return Serializer::read<PresetBankMetadataSerializer>(transaction, bankFolder, ".metadata", this, false);
}

void Bank::loadPresets(UNDO::Transaction *transaction, Glib::RefPtr<Gio::File> bankFolder)
{
  m_presets.forEach([&](Preset *p) { p->load(transaction, bankFolder); });
}

UpdateDocumentContributor::tUpdateID Bank::onChange(uint64_t flags)
{
  auto ret = AttributesOwner::onChange(flags);
  m_sigBankChanged.send();
  return ret;
}

const Uuid &Bank::getUuid() const
{
  return m_uuid;
}

std::string Bank::getName(bool withFallback) const
{
  if(m_name.empty() && withFallback)
    return "<Untitled Bank>";

  return m_name;
}

std::string Bank::getX() const
{
  return m_x;
}

std::string Bank::getY() const
{
  return m_y;
}

const Uuid &Bank::getSelectedPresetUuid() const
{
  return m_presets.getSelectedUuid();
}

Preset *Bank::getSelectedPreset() const
{
  return findPreset(getSelectedPresetUuid());
}

const Uuid &Bank::getAttachedToBankUuid() const
{
  return m_attachedToBankWithUuid;
}

std::string Bank::getAttachDirection() const
{
  return m_attachDirection;
}

size_t Bank::getNumPresets() const
{
  return m_presets.size();
}

size_t Bank::getPresetPosition(const Uuid &uuid) const
{
  return m_presets.getIndexOf(uuid);
}

size_t Bank::getPresetPosition(const Preset *preset) const
{
  return m_presets.getIndexOf(preset);
}

Preset *Bank::findPreset(const Uuid &uuid) const
{
  return m_presets.find(uuid);
}

Preset *Bank::findPresetNear(const Uuid &anchorUuid, int seek) const
{
  return m_presets.findNear(anchorUuid, seek);
}

Preset *Bank::findSelectedPreset() const
{
  return m_presets.getSelected();
}

Preset *Bank::getPresetAt(size_t idx) const
{
  if(!m_presets.empty())
    return m_presets.at(idx);
  return nullptr;
}

void Bank::forEachPreset(std::function<void(Preset *)> cb) const
{
  m_presets.forEach(cb);
}

Bank *Bank::getMasterTop() const
{
  if(toAttachDirection(getAttachDirection()) == AttachmentDirection::top)
    if(auto pm = dynamic_cast<const PresetManager *>(getParent()))
      return pm->findBank(getAttachedToBankUuid());

  return nullptr;
}

Bank *Bank::getMasterLeft() const
{
  if(toAttachDirection(getAttachDirection()) == AttachmentDirection::left)
    if(auto pm = dynamic_cast<const PresetManager *>(getParent()))
      return pm->findBank(getAttachedToBankUuid());

  return nullptr;
}

Bank *Bank::getSlaveRight() const
{
  if(auto pm = dynamic_cast<const PresetManager *>(getParent()))
    for(const auto bank : pm->getBanks())
      if(auto masterLeft = bank->getMasterLeft())
        if(masterLeft == this)
          return bank;

  return nullptr;
}

Bank *Bank::getSlaveBottom() const
{
  if(auto pm = dynamic_cast<const PresetManager *>(getParent()))
    for(const auto bank : pm->getBanks())
      if(auto masterLeft = bank->getMasterTop())
        if(masterLeft == this)
          return bank;

  return nullptr;
}

void Bank::selectNextPreset()
{
  selectPreset(getNextPresetPosition());
}

void Bank::selectPreviousPreset()
{
  selectPreset(getPreviousPresetPosition());
}

void Bank::selectNextPreset(UNDO::Transaction *transaction)
{
  selectPreset(transaction, getNextPresetPosition());
}

void Bank::selectPreviousPreset(UNDO::Transaction *transaction)
{
  selectPreset(transaction, getPreviousPresetPosition());
}

void Bank::selectPreset(UNDO::Transaction *transaction, size_t pos)
{
  if(pos < getNumPresets())
    selectPreset(transaction, getPresetAt(pos)->getUuid());
}

void Bank::selectPreset(size_t pos)
{
  if(pos < getNumPresets())
  {
    auto scope = getUndoScope().startTransaction("Select Preset '%0'", getPresetAt(pos)->getName());
    selectPreset(scope->getTransaction(), pos);
  }
}

size_t Bank::getNextPresetPosition() const
{
  return m_presets.getNextPosition(getSelectedPresetUuid());
}

size_t Bank::getPreviousPresetPosition() const
{
  return m_presets.getPreviousPosition(getSelectedPresetUuid());
}

void Bank::rename(const Glib::ustring &name)
{
  auto scope = getUndoScope().startTransaction("Rename Preset '%0'", name);
  setName(scope->getTransaction(), name);
}

void Bank::attachBank(UNDO::Transaction *transaction, const Uuid &otherBank, Bank::AttachmentDirection dir)
{
  setAttachedToBank(transaction, otherBank);
  setAttachedDirection(transaction, to_string(dir));
}

void Bank::invalidate()
{
  onChange(ChangeFlags::Generic);
}

void Bank::setName(UNDO::Transaction *transaction, const std::string &name)
{
  transaction->addUndoSwap(this, m_name, name);
  updateLastModifiedTimestamp(transaction);
}

void Bank::setUuid(UNDO::Transaction *transaction, const Uuid &uuid)
{
#if _DEVELOPMENT_PC
  if(auto existing = Application::get().getPresetManager()->findBank(uuid))
    nltools_assertOnDevPC(existing == this);
#endif

  transaction->addUndoSwap(this, m_uuid, uuid);
  updateLastModifiedTimestamp(transaction);
}

void Bank::selectPreset(UNDO::Transaction *transaction, const Uuid &uuid)
{
  if(m_presets.select(transaction, uuid))
    static_cast<PresetManager *>(getParent())->onPresetSelectionChanged();
}

void Bank::ensurePresetSelection(UNDO::Transaction *transaction)
{
  if(getNumPresets() && !findPreset(getSelectedPresetUuid()))
    selectPreset(transaction, m_presets.first()->getUuid());
}

void Bank::setAttachedToBank(UNDO::Transaction *transaction, const Uuid &uuid)
{
  transaction->addUndoSwap(this, m_attachedToBankWithUuid, uuid);
}

void Bank::setAttachedDirection(UNDO::Transaction *transaction, const std::string &direction)
{
  try
  {
    auto i = std::stoi(direction);
    auto dir = static_cast<Bank::AttachmentDirection>(i);
    transaction->addUndoSwap(this, m_attachDirection, to_string(dir));
  }
  catch(...)
  {
    transaction->addUndoSwap(this, m_attachDirection, direction);
  }
}

void Bank::setX(UNDO::Transaction *transaction, const std::string &x)
{
  transaction->addUndoSwap(this, m_x, x);
}

void Bank::setY(UNDO::Transaction *transaction, const std::string &y)
{
  transaction->addUndoSwap(this, m_y, y);
}

void Bank::updateLastModifiedTimestamp(UNDO::Transaction *transaction)
{
  transaction->addSimpleCommand([&](auto) { m_lastChangedTimestamp = TimeTools::getAdjustedTimestamp(); });
}

Preset *Bank::appendPreset(UNDO::Transaction *transaction)
{
  updateLastModifiedTimestamp(transaction);
  auto newPreset = m_presets.append(transaction, std::make_unique<Preset>(this));
  getPresetManager()->onPresetStored();
  return newPreset;
}

Preset *Bank::appendPreset(UNDO::Transaction *transaction, std::unique_ptr<Preset> preset)
{
  updateLastModifiedTimestamp(transaction);
  auto newPreset = m_presets.append(transaction, std::move(preset));
  getPresetManager()->onPresetStored();
  return newPreset;
}

Preset *Bank::appendAndLoadPreset(UNDO::Transaction *transaction, std::unique_ptr<Preset> preset)
{
  auto newPreset = appendPreset(transaction, std::move(preset));
  getEditBuffer()->undoableLoad(transaction, newPreset, false);
  getPresetManager()->onPresetStored();
  return newPreset;
}

Preset *Bank::prependPreset(UNDO::Transaction *transaction, std::unique_ptr<Preset> preset)
{
  updateLastModifiedTimestamp(transaction);
  auto newPreset = m_presets.prepend(transaction, std::move(preset));
  getPresetManager()->onPresetStored();
  return newPreset;
}

Preset *Bank::prependAndLoadPreset(UNDO::Transaction *transaction, std::unique_ptr<Preset> preset)
{
  auto newPreset = prependPreset(transaction, std::move(preset));
  getEditBuffer()->undoableLoad(transaction, newPreset, false);
  getPresetManager()->onPresetStored();
  return newPreset;
}

Preset *Bank::insertPreset(UNDO::Transaction *transaction, size_t pos, std::unique_ptr<Preset> preset)
{
  updateLastModifiedTimestamp(transaction);
  return m_presets.insert(transaction, pos, std::move(preset));
}

Preset *Bank::insertAndLoadPreset(UNDO::Transaction *transaction, size_t pos, std::unique_ptr<Preset> preset)
{
  auto newPreset = insertPreset(transaction, pos, std::move(preset));
  getEditBuffer()->undoableLoad(transaction, newPreset, false);
  getPresetManager()->onPresetStored();
  return newPreset;
}

void Bank::movePreset(UNDO::Transaction *transaction, const Preset *toMove, const Preset *before)
{
  updateLastModifiedTimestamp(transaction);
  m_presets.move(transaction, toMove, before);
}

void Bank::movePresetBetweenBanks(UNDO::Transaction *transaction, Preset *presetToMove, Bank *tgtBank,
                                  const Preset *presetAnchor)
{
  if(tgtBank == this)
  {
    movePreset(transaction, presetToMove, presetAnchor);
  }
  else
  {
    auto p = m_presets.release(transaction, presetToMove);
    nltools_assertAlways(p == presetToMove);
    auto pos = presetAnchor ? tgtBank->getPresetPosition(presetAnchor) : tgtBank->getNumPresets();
    tgtBank->m_presets.adopt(transaction, pos, p);
    tgtBank->updateLastModifiedTimestamp(transaction);
    updateLastModifiedTimestamp(transaction);
  }
}

void Bank::deletePreset(UNDO::Transaction *transaction, const Uuid &uuid)
{
  m_presets.remove(transaction, uuid);
  updateLastModifiedTimestamp(transaction);
}

sigc::connection Bank::onBankChanged(sigc::slot<void> cb)
{
  return m_sigBankChanged.connectAndInit(cb);
}

PresetManager *Bank::getPresetManager() const
{
  return static_cast<PresetManager *>(getParent());
}

void Bank::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  auto pm = dynamic_cast<const PresetManager *>(getParent());
  bool changed = knownRevision < getUpdateIDOfLastChange();

  writer.writeTag("preset-bank", Attribute("uuid", m_uuid.raw()), Attribute("name", m_name), Attribute("x", getX()),
                  Attribute("y", getY()), Attribute("selected-preset", getSelectedPresetUuid().raw()),
                  Attribute("order-number", pm->getBankPosition(getUuid()) + 1), Attribute("changed", changed), [&]() {
                    if(changed)
                    {
                      AttributesOwner::writeDocument(writer, knownRevision);
                      forEachPreset([&](auto p) { p->writeDocument(writer, knownRevision); });
                      writer.writeTextElement("date-of-last-change",
                                              TimeTools::getDisplayStringFromStamp(m_lastChangedTimestamp));
                      writer.writeTextElement("attached-to", getAttachedToBankUuid().raw());
                      writer.writeTextElement("attached-direction", getAttachDirection());
                      writer.writeTextElement("state", calcStateString());
                    }
                  });
}

bool Bank::empty() const
{
  return m_presets.empty();
}

void Bank::copyFrom(UNDO::Transaction *transaction, const Bank *other, bool ignoreUuids)
{
  if(!ignoreUuids)
    setUuid(transaction, other->getUuid());

  setName(transaction, other->getName(false));
  setX(transaction, other->getX());
  setY(transaction, other->getY());

  AttributesOwner::copyFrom(transaction, other);
  other->forEachPreset([&](auto p) { m_presets.append(transaction, std::make_unique<Preset>(this, p, ignoreUuids)); });

  updateLastModifiedTimestamp(transaction);
}

std::string to_string(Bank::AttachmentDirection dir)
{
  static_assert(Bank::AttachmentDirection::none == 0,
                "Nicht den Enum ändern ohne diese Funktion und Java Seite zu ändern!");
  static_assert(Bank::AttachmentDirection::top == 1,
                "Nicht den Enum ändern ohne diese Funktion und Java Seite zu ändern!");
  static_assert(Bank::AttachmentDirection::left == 2,
                "Nicht den Enum ändern ohne diese Funktion und Java Seite zu ändern!");
  static_assert(Bank::AttachmentDirection::count == 3,
                "Nicht den Enum ändern ohne diese Funktion und Java Seite zu ändern!");

  switch(dir)
  {
    case Bank::AttachmentDirection::top:
      return "top";

    case Bank::AttachmentDirection::left:
      return "left";

    case Bank::AttachmentDirection::none:
      return "";

    case Bank::AttachmentDirection::count:
      g_warn_if_reached();
  }
  return "";
}

Bank::AttachmentDirection toAttachDirection(const std::string &str)
{
  if(str == "top")
    return Bank::AttachmentDirection::top;

  if(str == "left")
    return Bank::AttachmentDirection::left;

  return Bank::AttachmentDirection::none;
}

int Bank::getHighestIncrementForBaseName(const Glib::ustring &baseName) const
{
  static auto regex = Glib::Regex::create("^(.*)\\s([0-9]+)$");

  bool hadMatch = false;
  int h = 0;

  m_presets.forEach([&](auto p) {
    Glib::ustring name = p->getName();

    if(name == baseName)
    {
      h = std::max(h, 1);
      hadMatch = true;
    }
    else if(name.find(baseName) == 0)
    {
      Glib::MatchInfo matchInfo;

      if(regex->match(name, matchInfo) && matchInfo.get_match_count() > 2)
      {
        auto presetsBaseName = matchInfo.fetch(1);

        if(presetsBaseName == baseName)
        {
          auto number = matchInfo.fetch(2);
          int newNumber = std::stoi(number);
          h = std::max(h, newNumber);
          hadMatch = true;
        }
      }
    }
  });

  if(hadMatch)
    return std::max(1, h);

  return 0;
}

void Bank::searchPresets(Writer &writer, const SearchQuery &query) const
{
  auto pm = dynamic_cast<const PresetManager *>(getParent());
  auto pos = pm->getBankPosition(getUuid());

  writer.writeTag("preset-bank", Attribute("uuid", m_uuid.raw()), Attribute("name", m_name),
                  Attribute("order-number", pos), [&]() {
                    m_presets.forEach([&](auto p) {
                      if(p->matchesQuery(query))
                        p->writeDocument(writer, 0);
                    });
                  });
}

time_t Bank::getLastChangedTimestamp() const
{
  return m_lastChangedTimestamp;
}

const Glib::ustring Bank::calcStateString() const
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

Bank *Bank::getClusterMaster()
{
  if(auto master = static_cast<PresetManager *>(getParent())->findBank(getAttachedToBankUuid()))
    if(master != this)
      return master->getClusterMaster();

  return this;
}

void Bank::resolveCyclicAttachments(UNDO::Transaction *transaction)
{
  std::vector<Bank *> banks;
  resolveCyclicAttachments(transaction, banks);
}

bool Bank::resolveCyclicAttachments(UNDO::Transaction *transaction, std::vector<Bank *> stackedBanks)
{
  if(std::find(stackedBanks.begin(), stackedBanks.end(), this) != stackedBanks.end())
  {
    return false;
  }

  stackedBanks.push_back(this);

  auto master = Application::get().getPresetManager()->findBank(getAttachedToBankUuid());

  if(master != nullptr && !master->resolveCyclicAttachments(transaction, stackedBanks))
  {
    setAttachedToBank(transaction, Uuid::none());
    setAttachedDirection(transaction, "");
  }
  stackedBanks.pop_back();
  return true;
}

const Preset *Bank::getFirstPreset() const
{
  if(m_presets.empty())
    return nullptr;
  return m_presets.at(0);
}

auto currentVG()
{
  return Application::get().getHWUI()->getCurrentVoiceGroup();
}
