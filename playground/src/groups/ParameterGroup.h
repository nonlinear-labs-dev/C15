#pragma once

#include "playground.h"
#include "parameters/Parameter.h"
#include "http/UpdateDocumentContributor.h"
#include "tools/IntrusiveList.h"

class PresetParameterGroup;

class ParameterGroup : public UpdateDocumentContributor, public IntrusiveListItem<ParameterGroup *>
{
 private:
  typedef UpdateDocumentContributor super;

 public:
  ParameterGroup(ParameterGroupSet *parent, const char *id, const char *shortName, const char *longName,
                 const char *webUIName);
  virtual ~ParameterGroup();

  virtual void init() = 0;

  Glib::ustring getShortName() const;
  Glib::ustring getLongName() const;

  typedef Parameter *tParameterPtr;

  size_t countParameters() const;
  tParameterPtr getParameterByID(gint32 id) const;
  tParameterPtr findParameterByID(gint32 id) const;

  const IntrusiveList<tParameterPtr> &getParameters() const
  {
    return m_parameters;
  }

  IntrusiveList<tParameterPtr> &getParameters()
  {
    return m_parameters;
  }

  void undoableClear(UNDO::Transaction *transaction);
  void undoableReset(UNDO::Transaction *transaction, Initiator initiator);

  void undoableLock(UNDO::Transaction *transaction);
  void undoableUnlock(UNDO::Transaction *transaction);
  void undoableToggleLock(UNDO::Transaction *transaction);
  bool areAllParametersLocked() const;
  bool isAnyParameterLocked() const;

  virtual void undoableRandomize(UNDO::Transaction *transaction, Initiator initiator, double amount);
  void undoableSetDefaultValues(UNDO::Transaction *transaction, const PresetParameterGroup *values);
  void undoableSetType(UNDO::Transaction *transaction, PresetType oldType, PresetType desiredType);


  Glib::ustring getID() const;
  size_t getHash() const;

  virtual void copyFrom(UNDO::Transaction *transaction, const PresetParameterGroup *other);

  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

  virtual tUpdateID onChange(uint64_t flags = UpdateDocumentContributor::ChangeFlags::Generic) override;

  // CALLBACKS
  sigc::connection onGroupChanged(slot<void> slot);

  void check();

 protected:
  tParameterPtr appendParameter(Parameter *p);

 private:
  const char *m_id;
  const char *m_shortName;
  const char *m_longName;
  const char *m_webUIName;
  IntrusiveList<tParameterPtr> m_parameters;
  Signal<void> m_signalGroupChanged;
};
