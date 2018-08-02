#include "Preset.h"
#include "PresetBank.h"
#include "PresetManager.h"
#include "EditBuffer.h"
#include "xml/Writer.h"
#include "libundo/undo/SwapData.h"
#include "http/UpdateDocumentMaster.h"
#include "Application.h"
#include "xml/FileInStream.h"
#include <xml/VersionAttribute.h>
#include <xml/CommitableFileOutStream.h>
#include <serialization/PresetSerializer.h>
#include <tools/PerformanceTimer.h>
#include <libundo/undo/TrashTransaction.h>
#include <device-settings/DebugLevel.h>
#include <device-settings/DeviceName.h>
#include <device-info/DateTimeInfo.h>
#include <device-settings/Settings.h>
#include <device-info/DeviceInformation.h>
#include "SearchQuery.h"
#include <testing/TestDriver.h>
#include <tools/TimeTools.h>

shared_ptr<Preset> Preset::createPreset(UpdateDocumentContributor *parent)
{
  shared_ptr<Preset> p(new Preset(parent));
  p->init();
  return p;
}

Preset::Preset(UpdateDocumentContributor *parent) :
    ParameterGroupSet(parent),
    m_settings(*this)
{
}

Preset::~Preset()
{
}

void Preset::setAutoGeneratedAttributes(UNDO::Scope::tTransactionPtr transaction)
{
  undoableSetAttribute(transaction, "DeviceName", Application::get().getSettings()->getSetting<DeviceName>()->get());
  undoableSetAttribute(transaction, "StoreTime", TimeTools::getAdjustedIso());
  undoableSetAttribute(transaction, "SoftwareVersion", Application::get().getDeviceInformation()->getSoftwareVersion());

  updateBankChangeTime();
}

void Preset::updateBankChangeTime()
{
  getBank()->onChange();
}

void Preset::load(UNDO::Scope::tTransactionPtr transaction, RefPtr<Gio::File> presetPath)
{
  Glib::ustring strUUID = getUuid();
  Serializer::read<PresetSerializer>(transaction, presetPath, strUUID, this);

  m_lastSavedUpdateID = getUpdateIDOfLastChange();
}

bool Preset::save(RefPtr<Gio::File> bankPath)
{
  if(m_lastSavedUpdateID != getUpdateIDOfLastChange())
  {
    PresetSerializer serializer(this);
    Glib::ustring strUUID = getUuid();
    serializer.write(bankPath, strUUID);
    m_lastSavedUpdateID = getUpdateIDOfLastChange();
    return true;
  }
  return false;
}

shared_ptr<const PresetBank> Preset::getBank() const
{
  if(auto b = dynamic_cast<const PresetBank *>(getParent()))
    return b->shared_from_this();

  return nullptr;
}

shared_ptr<PresetBank> Preset::getBank()
{
  if(auto b = dynamic_cast<PresetBank *>(getParent()))
    return b->shared_from_this();

  return nullptr;
}

void Preset::setName(const ustring &name)
{
  m_name = name;
}

void Preset::undoableSetName(const ustring &name)
{
  if(name != m_name)
  {
    UNDO::Scope::tTransactionScopePtr scope = getUndoScope().startTransaction("Rename preset '%0' to '%1'", getName(), name);
    undoableSetName(scope->getTransaction(), name);
  }
}

void Preset::undoableSetName(UNDO::Scope::tTransactionPtr transaction, const ustring &name)
{
  if(name != m_name)
  {
    auto swapData = UNDO::createSwapData(name);

    transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
    {
      swapData->swapWith (m_name);
      onChange ();
    });

    Application::get().getPresetManager()->getEditBuffer()->undoableUpdateLoadedPresetInfo(transaction);
  }
}

void Preset::undoableSetType(UNDO::Scope::tTransactionPtr transaction, PresetType desiredType)
{
  if(m_type != desiredType)
  {
    for(auto g : getParameterGroups())
    {
      g->undoableSetType(transaction, m_type, desiredType);
    }

    auto swapData = UNDO::createSwapData(desiredType);

    transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
    {
      swapData->swapWith (m_type);
    });
  }
}

Parameter* Preset::getParameterByID(gint32 ID) const {
  return getParametersSortedById().at(ID);
}


void Preset::undoableSetUuid(UNDO::Scope::tTransactionPtr transaction, const Uuid &uuid)
{
  bool updateLastLoadedPresetInfo = false;

  if(auto bank = getBank())
  {
    auto pm = bank->getParent();
    auto eb = pm->getEditBuffer();
    updateLastLoadedPresetInfo = eb->getUUIDOfLastLoadedPreset() == getUuid();
  }

  auto swapData = UNDO::createSwapData(uuid);

  transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
  {
    swapData->swapWith (m_uuid);
    onChange();
  });

  if(updateLastLoadedPresetInfo)
  {
    auto bank = getBank();
    auto pm = bank->getParent();
    auto eb = pm->getEditBuffer();
    eb->undoableSetLoadedPresetInfo(transaction, this);
  }
}

UpdateDocumentContributor::tUpdateID Preset::onChange(uint64_t flags)
{
  if(getParent())
  {
    auto u = ParameterGroupSet::onChange(flags);
    m_signalPresetChanged.send();
    return u;
  }

  return 0;
}

ustring Preset::getName() const
{
  return m_name;
}

const Uuid &Preset::getUuid() const
{
  return m_uuid;
}

void Preset::setUuid(const Uuid &uuid)
{
  m_uuid = uuid;
}

sigc::connection Preset::onPresetChanged(slot<void> s)
{
  return m_signalPresetChanged.connectAndInit(s);
}

void Preset::writeDocument(Writer &writer, tUpdateID knownRevision) const
{
  bool changed = knownRevision < getUpdateIDOfLastChange();

  writer.writeTag("preset", Attribute("uuid", (Glib::ustring) m_uuid), Attribute("name", m_name), Attribute("changed", changed), [&]()
  {
    if (changed)
    {
      writeAttributes(writer);
      m_settings.writeDocument (writer, knownRevision);
    }
  });
}

void Preset::copyFrom(UNDO::Scope::tTransactionPtr transaction, Preset *other, bool ignoreUUIDs)
{
  ParameterGroupSet::copyFrom(transaction, other);

  if(auto preset = other)
  {
    m_settings.copyFrom(transaction, preset->getSettings());
    AttributesOwner::copyFrom(transaction, preset);

    auto swapData = UNDO::createSwapData(preset->m_name);

    transaction->addSimpleCommand([ = ] (UNDO::Command::State) mutable
    {
      swapData->swapWith (m_name);
      onChange ();
    });

    if(!ignoreUUIDs)
      undoableSetUuid(transaction, other->getUuid());
  }
}

void Preset::undoableSelect(UNDO::Scope::tTransactionPtr transaction)
{
  getBank()->undoableSelectPreset(transaction, getUuid());
}

void Preset::guessName(UNDO::Scope::tTransactionPtr transaction)
{
  Glib::ustring currentName = getName();

  if(currentName.empty())
    currentName = "New preset";

  undoableSetName(transaction, "");
  Glib::ustring newName = getBank()->getParent()->createPresetNameBasedOn(currentName);
  undoableSetName(transaction, newName);
}

PresetSettings &Preset::getSettings()
{
  return m_settings;
}

const PresetSettings &Preset::getSettings() const
{
  return m_settings;
}

size_t Preset::getHash() const
{
  size_t hash = AttributesOwner::getHash();
  hash_combine(hash, m_settings.getHash());

  for(auto g : getParameterGroups())
  {
    hash_combine(hash, g->getHash());
  }

  return hash;
}

bool Preset::hasDuplicateName() const
{
  size_t num = 0;

  for(auto bank : Application::get().getPresetManager()->getBanks())
  {
    for(auto preset : bank->getPresets())
    {
      if(preset->getName() == getName())
      {
        num++;

        if(num > 1)
        {
          return true;
        }
      }
    }
  }

  return false;
}

Glib::ustring Preset::getUndoTransactionTitle(const Glib::ustring &prefix) const
{
  if(auto bank = getBank())
  {
    int bankNumber = bank->getParent()->calcOrderNumber(bank.get());
    int presetNumber = bank->getPresetPosition(getUuid()) + 1;
    char txt[256];
    sprintf(txt, "%d-%03d", bankNumber, presetNumber);
    return UNDO::StringTools::buildString(prefix, " ", txt, ": '", getName(), "'");
  }

  return UNDO::StringTools::buildString(prefix, " '", getName(), "'");
}

bool Preset::matchesQuery(const SearchQuery &query) const
{
  return query.iterate([&](const Glib::ustring &part, std::vector<PresetManager::presetInfoSearchFields> fields)
  {
    static_assert((int)PresetManager::presetInfoSearchFields::name == 0, "");
    static_assert((int)PresetManager::presetInfoSearchFields::comment == 1, "");
    static_assert((int)PresetManager::presetInfoSearchFields::devicename == 2, "");

    std::array<ustring, 3> entries =
    {
      getName().lowercase(),
      getAttribute("Comment", "").lowercase(),
      getAttribute("DeviceName", "").lowercase()
    };

    for(auto f : fields)
    {
      if(entries[(int)f].find(part) != ustring::npos)
      {
        return true;
      }
    }
    return false;
  });
}

void Preset::writeDiff(Writer &writer, Preset *other) const
{
  char txt[256];
  auto pm = Application::get().getPresetManager();

  Glib::ustring aPositionString = "Edit Buffer";
  Glib::ustring bPositionString = "Edit Buffer";

  if(auto b = getBank())
  {
    sprintf(txt, "%d-%03d", pm->calcOrderNumber(b.get()), b->getPresetPosition(getUuid()) + 1);
    aPositionString = txt;
  }

  if(auto b = other->getBank())
  {
    sprintf(txt, "%d-%03d", pm->calcOrderNumber(b.get()), b->getPresetPosition(other->getUuid()) + 1);
    bPositionString = txt;
  }

  writer.writeTag("diff",
      [&]
      {
        auto hash = getHash();
        auto otherHash = other->getHash();

        writer.writeTextElement("hash", "", Attribute("a", hash), Attribute("b", otherHash));
        writer.writeTextElement("position", "", Attribute("a", aPositionString), Attribute("b", bPositionString));
        writer.writeTextElement("name", "", Attribute("a", getName()), Attribute("b", other->getName()));

        if(hash != otherHash)
        {
          if(getAttribute("Comment", "") != other->getAttribute("Comment", ""))
          {
            writer.writeTextElement("comment", "", Attribute("a", getAttribute("Comment", "")), Attribute("b", other->getAttribute("Comment", "")));
          }

          if(getAttribute("color", "") != other->getAttribute("color", ""))
          {
            writer.writeTextElement("color", "", Attribute("a", getAttribute("color", "")), Attribute("b", other->getAttribute("color", "")));
          }

          ParameterGroupSet::writeDiff(writer, other);
        }
      });
}
