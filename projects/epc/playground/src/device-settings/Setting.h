#pragma once

#include <playground.h>
#include <http/UpdateDocumentContributor.h>
#include <tools/Signal.h>
#include "SendReason.h"

class Settings;

class Setting : public UpdateDocumentContributor
{
 private:
  typedef UpdateDocumentContributor super;

 public:
  explicit Setting(UpdateDocumentContributor &parent);
  ~Setting() override;

  virtual void init();
  virtual void setSetting(Initiator initiator, const Glib::ustring &text);
  virtual void load(const Glib::ustring &text, Initiator initiator) = 0;
  virtual Glib::ustring save() const = 0;

  sigc::connection onChange(sigc::slot<void, const Setting *> slot);
  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

  virtual void syncExternals(SendReason reason = SendReason::SettingChanged) const;
  virtual bool persistent() const;

  virtual Glib::ustring getDisplayString() const = 0;

 protected:
  void notify();

 private:
  Signal<void, const Setting *> m_signal;
};
