#pragma once

#include "playground.h"
#include "http/UpdateDocumentContributor.h"

class Settings;

class Setting : public UpdateDocumentContributor
{
 private:
  typedef UpdateDocumentContributor super;

 public:
  Setting(Settings &parent);
  virtual ~Setting();

  virtual void setSetting(Initiator initiator, const Glib::ustring &text);
  virtual void load(const Glib::ustring &text) = 0;
  virtual Glib::ustring save() const = 0;

  connection onChange(slot<void, const Setting *> slot);
  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

  virtual void sendToLPC() const;

 protected:
  void notify();

 private:
  Signal<void, const Setting *> m_signal;
};
