#include "Clipboard.h"
#include "MultiplePresetSelection.h"
#include <presets/PresetManager.h>
#include <presets/Preset.h>
#include <presets/Bank.h>
#include <xml/Attribute.h>
#include <xml/Writer.h>
#include <Application.h>
#include <tools/StringTools.h>
#include <device-settings/DebugLevel.h>
#include <http/NetworkRequest.h>
#include <giomm/file.h>

Clipboard::Clipboard(UpdateDocumentContributor *parent)
    : ContentSection(parent)
    , m_actions("/clipboard/")
{
  m_actions.addAction("copy-bank", [=](std::shared_ptr<NetworkRequest> request) {
    copyBank(Uuid { request->get("bank") });
    request->okAndComplete();
  });

  m_actions.addAction("cut-preset", [=](std::shared_ptr<NetworkRequest> request) {
    cutPreset(Uuid { request->get("preset") });
    request->okAndComplete();
  });

  m_actions.addAction("copy-preset", [=](std::shared_ptr<NetworkRequest> request) {
    copyPreset(Uuid { request->get("preset") });
    request->okAndComplete();
  });

  m_actions.addAction("copy-presets", [=](std::shared_ptr<NetworkRequest> request) {
    copyPresets(request->get("presets-csv"));
    request->okAndComplete();
  });

  m_actions.addAction("paste-on-background", [=](std::shared_ptr<NetworkRequest> request) {
    auto x = request->get("x");
    auto y = request->get("y");

    if(containsBank())
      pasteBankOnBackground("Paste Bank", x, y);
    else if(containsPreset())
      pastePresetOnBackground(x, y);
    else if(containsMultiplePresets())
      pasteMultiplePresetsOnBackground(x, y);

    request->okAndComplete();
  });

  m_actions.addAction("paste-on-bank", [=](std::shared_ptr<NetworkRequest> request) {
    if(containsBank())
      pasteBankOnBank("Paste Bank", Uuid { request->get("bank") });
    else if(containsPreset())
      pastePresetOnBank(Uuid { request->get("bank") });
    else if(containsMultiplePresets())
      pasteMultiplePresetsOnBank(Uuid { request->get("bank") });

    request->okAndComplete();
  });

  m_actions.addAction("paste-on-preset", [=](std::shared_ptr<NetworkRequest> request) {
    if(containsBank())
      pasteBankOnPreset("Paste Bank", Uuid { request->get("preset") });
    else if(containsPreset())
      pastePresetOnPreset(Uuid { request->get("preset") });
    else if(containsMultiplePresets())
      pasteMultiplePresetsOnPreset(Uuid { request->get("preset") });
    request->okAndComplete();
  });
}

Clipboard::~Clipboard()
{
  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
}

void Clipboard::handleHTTPRequest(std::shared_ptr<NetworkRequest> request, const Glib::ustring &path)
{
  ContentSection::handleHTTPRequest(request, path);

  if(m_actions.matches(path))
    if(m_actions.handleRequest(path, request))
      return;
}

Glib::ustring Clipboard::getPrefix() const
{
  return "clipboard";
}

bool Clipboard::containsMultiplePresets() const
{
  return dynamic_cast<MultiplePresetSelection *>(m_content.get()) != nullptr;
}

bool Clipboard::containsBank() const
{
  return dynamic_cast<Bank *>(m_content.get()) != nullptr;
}

bool Clipboard::containsPreset() const
{
  return dynamic_cast<Preset *>(m_content.get()) != nullptr;
}

bool Clipboard::hasContent() const
{
  return containsBank() || containsPreset() || containsMultiplePresets();
}

void Clipboard::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  bool changed = knownRevision < getUpdateIDOfLastChange();

  writer.writeTag("clipboard", Attribute("changed", changed), [&]() {
    if(changed)
    {
      writer.writeTextElement("contains-bank", to_string(containsBank()));
      writer.writeTextElement("contains-preset", to_string(containsPreset()));
      writer.writeTextElement("contains-multiple-presets", to_string(containsMultiplePresets()));
    }
  });
}

void Clipboard::copyBank(const Uuid &bankUuid)
{
  auto pm = Application::get().getPresetManager();
  if(auto bank = pm->findBank(bankUuid))
  {
    m_currentContentWasCut = false;
    auto scope = getUndoScope().startTrashTransaction();
    m_content = std::make_unique<Bank>(this, *bank, false);
    onChange(UpdateDocumentContributor::ChangeFlags::Generic);
  }
}

void Clipboard::copyPresets(const Glib::ustring &csv)
{
  auto pm = Application::get().getPresetManager();
  auto scope = getUndoScope().startTrashTransaction();
  m_currentContentWasCut = false;
  m_content.reset(new MultiplePresetSelection(getParent()));

  auto mulPresetSelection = static_cast<MultiplePresetSelection *>(m_content.get());

  for(const auto &uuid : StringTools::splitStringOnAnyDelimiter(csv, ','))
  {
    if(auto preset = pm->findPreset(Uuid { uuid }))
    {
      mulPresetSelection->addPreset(scope->getTransaction(), preset);
    }
  }
  onChange(UpdateDocumentContributor::ChangeFlags::Generic);
}

bool Clipboard::copyPreset(const Uuid &presetUuid)
{
  auto pm = Application::get().getPresetManager();

  if(auto preset = pm->findPreset(presetUuid))
  {
    m_currentContentWasCut = false;
    auto scope = getUndoScope().startTrashTransaction();
    m_content = std::make_unique<Preset>(this, *preset, false);
    onChange(UpdateDocumentContributor::ChangeFlags::Generic);
    return true;
  }
  return false;
}

void Clipboard::cutPreset(const Uuid &presetUuid)
{
  if(copyPreset(presetUuid))
    m_currentContentWasCut = true;
}

void Clipboard::pasteOnBank(const Uuid &bankUuid)
{
  if(containsPreset())
    pastePresetOnBank(bankUuid);
  else if(containsBank())
    pasteBankOnBank("Paste Bank", bankUuid);
  else if(containsMultiplePresets())
    pasteMultiplePresetsOnBank(bankUuid);
}

void Clipboard::pasteOnPreset(const Uuid &presetUuid)
{
  if(containsPreset())
    pastePresetOnPreset(presetUuid);
  else if(containsBank())
    pasteBankOnPreset("Paste Bank", presetUuid);
  else if(containsMultiplePresets())
    pasteMultiplePresetsOnPreset(presetUuid);
}

void Clipboard::pasteBankOnBackground(const Glib::ustring &transactionName, const Glib::ustring &x,
                                      const Glib::ustring &y)
{
  pasteBankOnBackground(transactionName, x, y, m_content.get());
}

void Clipboard::pasteBankOnBackground(const Glib::ustring &transactionName, const Glib::ustring &x,
                                      const Glib::ustring &y, const UpdateDocumentContributor *content)
{
  auto pm = Application::get().getPresetManager();

  auto srcBank = dynamic_cast<const Bank *>(content);
  PresetManagerUseCases useCase(pm);
  useCase.pasteBankOnBackground(transactionName, x, y, srcBank, this);
}

std::unique_ptr<Bank> multiplePresetsToBank(const MultiplePresetSelection &mulPresets)
{
  auto scope = Application::get().getClipboard()->getUndoScope().startTrashTransaction();
  auto transaction = scope->getTransaction();
  auto b = std::make_unique<Bank>(Application::get().getPresetManager());

  for(auto &preset : mulPresets.getPresets())
    b->prependPreset(transaction, std::make_unique<Preset>(b.get(), *preset.get(), true));

  b->ensurePresetSelection(transaction);
  return b;
}

void Clipboard::pasteMultiplePresetsOnBackground(const Glib::ustring &x, const Glib::ustring &y)
{
  auto multiplePresets = static_cast<MultiplePresetSelection *>(m_content.get());
  auto bank = multiplePresetsToBank(*multiplePresets);
  pasteBankOnBackground("Paste Multiple Presets", x, y, bank.get());
}

void Clipboard::pasteMultiplePresetsOnBank(const Uuid &bankUuid)
{
  auto multiplePresets = static_cast<MultiplePresetSelection *>(m_content.get());
  auto bank = multiplePresetsToBank(*multiplePresets);
  pasteBankOnBank("Paste Multiple Presets", bankUuid, bank.get());
}

void Clipboard::pasteMultiplePresetsOnPreset(const Uuid &presetUuid)
{
  auto multiplePresets = static_cast<MultiplePresetSelection *>(m_content.get());
  auto bank = multiplePresetsToBank(*multiplePresets);
  pasteBankOnPreset("Paste Multiple Presets", presetUuid, bank.get());
}

void Clipboard::pastePresetOnBackground(const Glib::ustring &x, const Glib::ustring &y)
{
  auto pm = Application::get().getPresetManager();
  auto srcPreset = dynamic_cast<Preset *>(m_content.get());
  PresetManagerUseCases useCases(pm);
  useCases.pastePresetOnBackground(x, y, srcPreset, this);
}

void Clipboard::pasteBankOnBank(const Glib::ustring &transactionName, const Uuid &bankUuid)
{
  pasteBankOnBank(transactionName, bankUuid, m_content.get());
}

void Clipboard::pasteBankOnBank(const Glib::ustring &transactionName, const Uuid &bankUuid,
                                const UpdateDocumentContributor *content)
{
  auto scope = getUndoScope().startTransaction(transactionName);
  auto pm = Application::get().getPresetManager();

  if(auto target = pm->findBank(bankUuid))
  {
    auto transaction = scope->getTransaction();
    auto source = dynamic_cast<const Bank *>(content);
    source->forEachPreset([&](auto preset) {
      auto newPreset = std::make_unique<Preset>(target, *preset, true);
      target->appendPreset(transaction, std::move(newPreset));
    });

    doCut(transaction);
  }
}

void Clipboard::pastePresetOnBank(const Uuid &bankUuid)
{
  if(containsPreset())
  {
    auto pm = Application::get().getPresetManager();

    if(auto target = pm->findBank(bankUuid))
    {
      PresetManagerUseCases useCase(pm);
      auto source = dynamic_cast<const Preset *>(m_content.get());
      useCase.pastePresetOnBank(target, source, this);
    }
  }
}

void Clipboard::pasteBankOnPreset(const Glib::ustring &transactionName, const Uuid &presetUuid)
{
  pasteBankOnPreset(transactionName, presetUuid, m_content.get());
}

void Clipboard::pasteBankOnPreset(const Glib::ustring &transactionName, const Uuid &presetUuid,
                                  const UpdateDocumentContributor *content)
{
  auto scope = getUndoScope().startTransaction(transactionName);
  auto transaction = scope->getTransaction();
  auto pm = Application::get().getPresetManager();

  if(auto targetPreset = pm->findPreset(presetUuid))
  {
    if(auto targetBank = static_cast<Bank *>(targetPreset->getParent()))
    {
      auto source = static_cast<const Bank *>(content);
      auto insertPos = targetBank->getPresetPosition(presetUuid) + 1;

      source->forEachPreset([&](auto srcPreset) {
        auto cp = std::make_unique<Preset>(targetBank, *srcPreset, true);
        targetBank->insertPreset(transaction, insertPos++, std::move(cp));
      });

      doCut(transaction);
    }
  }
}

void Clipboard::pastePresetOnPreset(const Uuid &presetUuid)
{
  auto pm = Application::get().getPresetManager();

  if(auto targetPreset = pm->findPreset(presetUuid))
  {
    PresetManagerUseCases useCase(pm);
    auto source = dynamic_cast<Preset *>(m_content.get());
    useCase.pastePresetOnPreset(targetPreset, source, this);
  }
}

void Clipboard::doCut(UNDO::Transaction *transaction)
{
  if(m_currentContentWasCut)
  {
    m_currentContentWasCut = false;

    if(auto p = dynamic_cast<const Preset *>(m_content.get()))
    {
      if(auto src = Application::get().getPresetManager()->findPreset(p->getUuid()))
      {
        if(auto parentBank = dynamic_cast<Bank *>(src->getParent()))
        {
          parentBank->deletePreset(transaction, p->getUuid());
        }
      }
    }
  }
}

UpdateDocumentContributor::tUpdateID Clipboard::onChange(uint64_t flags)
{
  auto ret = super::onChange(flags);
  m_sigChanged.send();
  return ret;
}

sigc::connection Clipboard::onClipboardChanged(sigc::slot<void> cb)
{
  return m_sigChanged.connectAndInit(cb);
}
