#include "Clipboard.h"
#include <presets/PresetManager.h>
#include <presets/Preset.h>
#include <presets/PresetBank.h>
#include <xml/Attribute.h>
#include <xml/Writer.h>
#include <Application.h>

Clipboard::Clipboard (UpdateDocumentContributor *parent) :
    ContentSection (parent),
    m_actions ("/clipboard/")
{
  m_actions.addAction ("copy-bank", [=](shared_ptr<NetworkRequest> request)
  {
    copyBank(request->get ("bank"));
    request->okAndComplete();
  });

  m_actions.addAction ("cut-preset", [=](shared_ptr<NetworkRequest> request)
  {
    cutPreset(request->get ("preset"));
    request->okAndComplete();
  });

  m_actions.addAction ("copy-preset", [=](shared_ptr<NetworkRequest> request)
  {
    copyPreset(request->get ("preset"));
    request->okAndComplete();
  });

  m_actions.addAction ("paste-on-background", [=](shared_ptr<NetworkRequest> request)
  {
    auto x = request->get("x");
    auto y = request->get("y");

    if(containsBank())
      pasteBankOnBackground(x, y);
    else if(containsPreset())
      pastePresetOnBackground(x, y);

    request->okAndComplete();
  });

  m_actions.addAction ("paste-on-bank", [=](shared_ptr<NetworkRequest> request)
  {
    if(containsBank())
    pasteBankOnBank(request->get ("bank"));
    else if(containsPreset())
    pastePresetOnBank(request->get ("bank"));

    request->okAndComplete();
  });

  m_actions.addAction ("paste-on-preset", [=](shared_ptr<NetworkRequest> request)
  {
    if(containsBank())
    pasteBankOnPreset(request->get ("preset"));
    else if(containsPreset())
    pastePresetOnPreset(request->get ("preset"));

    request->okAndComplete();
  });
}

Clipboard::~Clipboard ()
{
}

void Clipboard::handleHTTPRequest (shared_ptr<NetworkRequest> request, const Glib::ustring &path)
{
  ContentSection::handleHTTPRequest (request, path);

  if (m_actions.matches (path))
    if (m_actions.handleRequest (path, request))
      return;
}

Glib::ustring Clipboard::getPrefix () const
{
  return "clipboard";
}

bool Clipboard::containsBank () const
{
  return dynamic_pointer_cast<PresetBank> (m_content) != nullptr;
}

bool Clipboard::containsPreset () const
{
  return dynamic_pointer_cast<Preset> (m_content) != nullptr;
}

bool Clipboard::hasContent() const
{
  return m_content.get();
}

void Clipboard::writeDocument (Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  bool changed = knownRevision < getUpdateIDOfLastChange ();

  writer.writeTag ("clipboard", Attribute ("changed", changed), [&]()
  {
    if (changed)
    {
      writer.writeTextElement("contains-bank", to_string(containsBank()));
      writer.writeTextElement("contains-preset", to_string(containsPreset()));
    }
  });
}

void Clipboard::copyBank (const Glib::ustring &bankUuid)
{
  auto pm = Application::get ().getPresetManager ();
  if (auto bank = pm->findBank (bankUuid))
  {
    m_currentContentWasCut = false;
    auto scope = getUndoScope ().startTrashTransaction ();
    m_content.reset (new PresetBank (pm.get ()));
    static_pointer_cast<PresetBank> (m_content)->copyFrom (scope->getTransaction (), bank, false);
    onChange (UpdateDocumentContributor::ChangeFlags::Generic);
  }
}

bool Clipboard::copyPreset (const Glib::ustring &presetUuid)
{
  auto pm = Application::get ().getPresetManager ();
  if (auto preset = pm->findPreset (presetUuid))
  {
    m_currentContentWasCut = false;
    auto scope = getUndoScope ().startTrashTransaction ();
    m_content = Preset::createPreset (nullptr);
    static_pointer_cast<Preset> (m_content)->copyFrom (scope->getTransaction (), preset.get (), false);
    onChange (UpdateDocumentContributor::ChangeFlags::Generic);
    return true;
  }
  return false;
}

void Clipboard::cutPreset (const Glib::ustring &presetUuid)
{
  if (copyPreset (presetUuid))
    m_currentContentWasCut = true;
}

void Clipboard::pasteOnBank (const Glib::ustring &bankUuid)
{
  if (containsPreset ())
    pastePresetOnBank (bankUuid);
  else if (containsBank ())
    pasteBankOnBank (bankUuid);
}

void Clipboard::pasteOnPreset (const Glib::ustring &presetUuid)
{
  if (containsPreset ())
    pastePresetOnPreset (presetUuid);
  else if (containsBank ())
    pasteBankOnPreset (presetUuid);
}

void Clipboard::pasteBankOnBackground (const Glib::ustring &x, const Glib::ustring &y)
{
  auto scope = getUndoScope ().startTransaction ("Paste Bank");
  auto pm = Application::get ().getPresetManager ();
  auto transaction = scope->getTransaction ();
  auto newBank = pm->addBank (transaction, true);
  newBank->copyFrom (transaction, dynamic_pointer_cast<PresetBank> (m_content), true);
  newBank->undoableSetPosition (transaction, x, y);

  doCut (transaction);
}

void Clipboard::pastePresetOnBackground (const Glib::ustring &x, const Glib::ustring &y)
{
  auto scope = getUndoScope ().startTransaction ("Paste Preset");
  auto pm = Application::get ().getPresetManager ();
  auto transaction = scope->getTransaction ();
  auto newBank = pm->addBank (transaction, true);
  newBank->undoableInsertPreset (transaction, 0);
  auto preset = newBank->getPreset (0);
  preset->copyFrom (transaction, dynamic_pointer_cast<Preset> (m_content).get (), true);
  newBank->undoableSetPosition (transaction, x, y);
  newBank->undoableEnsurePresetSelection(transaction);
  doCut (transaction);
}

void Clipboard::pasteBankOnBank (const Glib::ustring &bankUuid)
{
  auto pm = Application::get ().getPresetManager ();

  if (auto target = pm->findBank (bankUuid))
  {
    auto source = dynamic_pointer_cast<PresetBank> (m_content);

    if (size_t numPresets = source->getNumPresets ())
    {
      auto scope = getUndoScope ().startTransaction ("Paste Bank");
      auto transaction = scope->getTransaction ();
      auto insertPos = target->getNumPresets ();

      for (size_t i = 0; i < numPresets; i++)
      {
        target->undoableInsertPreset (transaction, insertPos);
        target->getPreset (insertPos)->copyFrom (transaction, source->getPreset (i).get (), true);
        insertPos++;
      }

      doCut (transaction);
    }
  }
}

void Clipboard::pastePresetOnBank (const Glib::ustring &bankUuid)
{
  if (containsPreset ())
  {
    auto pm = Application::get ().getPresetManager ();

    if (auto target = pm->findBank (bankUuid))
    {
      auto scope = getUndoScope ().startTransaction ("Paste Preset");
      auto transaction = scope->getTransaction ();

      auto source = dynamic_pointer_cast<Preset> (m_content);
      auto insertPos = target->getNumPresets ();
      target->undoableInsertPreset (transaction, insertPos);
      target->getPreset (insertPos)->copyFrom (transaction, source.get (), true);

      doCut (transaction);
    }
  }
}

void Clipboard::pasteBankOnPreset (const Glib::ustring &presetUuid)
{
  auto pm = Application::get ().getPresetManager ();

  if (auto targetPreset = pm->findPreset (presetUuid))
  {
    if (auto targetBank = targetPreset->getBank ())
    {
      auto source = dynamic_pointer_cast<PresetBank> (m_content);

      if (size_t numPresets = source->getNumPresets ())
      {
        auto scope = getUndoScope ().startTransaction ("Paste Bank");
        auto transaction = scope->getTransaction ();
        auto insertPos = targetBank->getPresetPosition (presetUuid);

        for (size_t i = 0; i < numPresets; i++)
        {
          targetBank->undoableInsertPreset (transaction, insertPos);
          targetBank->getPreset (insertPos)->copyFrom (transaction, source->getPreset (i).get (), true);
          insertPos++;
        }

        doCut (transaction);
      }
    }
  }
}

void Clipboard::pastePresetOnPreset (const Glib::ustring &presetUuid)
{
  auto pm = Application::get ().getPresetManager ();

  if (auto targetPreset = pm->findPreset (presetUuid))
  {
    if (auto targetBank = targetPreset->getBank ())
    {
      auto scope = getUndoScope ().startTransaction ("Paste Preset");
      auto transaction = scope->getTransaction ();

      auto insertPos = targetBank->getPresetPosition (presetUuid) + 1;
      auto source = dynamic_pointer_cast<Preset> (m_content);
      targetBank->undoableInsertPreset (transaction, insertPos);
      auto newPreset = targetBank->getPreset (insertPos);
      newPreset->copyFrom (transaction, source.get (), true);
      newPreset->undoableSelect (transaction);
      doCut (transaction);
    }
  }
}

void Clipboard::doCut (std::shared_ptr<UNDO::Transaction> transaction)
{
  if (m_currentContentWasCut)
  {
    m_currentContentWasCut = false;

    if (auto p = static_pointer_cast<Preset> (m_content))
    {
      auto uuid = p->getUuid ();

      if (auto src = Application::get ().getPresetManager ()->findPreset (uuid))
      {
        if (auto parentBank = src->getBank ())
        {
          parentBank->undoableDeletePreset (transaction, uuid);
        }
      }
    }
  }
}

UpdateDocumentContributor::tUpdateID Clipboard::onChange (uint64_t flags)
{
  auto ret = super::onChange(flags);
  m_sigChanged.send();
  return ret;
}

connection Clipboard::onClipboardChanged(slot<void> cb)
{
  return m_sigChanged.connectAndInit(cb);
}

