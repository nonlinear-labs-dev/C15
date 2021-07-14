#pragma once

#include "playground.h"
#include "GroupId.h"
#include "parameters/Parameter.h"
#include "http/UpdateDocumentContributor.h"
#include "tools/IntrusiveList.h"

class PresetParameterGroup;
class ParameterGroupSet;

class ParameterGroup : public UpdateDocumentContributor, public IntrusiveListItem<ParameterGroup *>
{
 private:
  typedef UpdateDocumentContributor super;

 public:
  ParameterGroup(ParameterGroupSet *parent, GroupId id, const char *shortName, const char *longName,
                 const char *webUIName);
  ~ParameterGroup() override;

  virtual void init() = 0;

  Glib::ustring getShortName() const;
  Glib::ustring getLongName() const;

  typedef Parameter *tParameterPtr;

  tParameterPtr getParameterByID(const ParameterId &id) const;

  tParameterPtr findParameterByID(const ParameterId &id) const;

  template <typename tParameterPtrT> tParameterPtrT *findAndCastParameterByID(const ParameterId &id) const
  {
    if(auto p = getParameterByID(id))
      return dynamic_cast<tParameterPtrT *>(p);
    return nullptr;
  }

  void forEachParameter(const std::function<void(Parameter *p)> &cb)
  {
    for(auto p : m_parameters)
      cb(p);
  }

  const IntrusiveList<tParameterPtr> &getParameters() const
  {
    return m_parameters;
  }

  IntrusiveList<tParameterPtr> &getParameters()
  {
    return m_parameters;
  }

  void undoableReset(UNDO::Transaction *transaction, Initiator initiator);

  void undoableLock(UNDO::Transaction *transaction);
  void undoableUnlock(UNDO::Transaction *transaction);
  void undoableToggleLock(UNDO::Transaction *transaction);
  bool areAllParametersLocked() const;
  bool isAnyParameterLocked() const;
  bool isAnyParameterChanged() const;

  virtual void undoableRandomize(UNDO::Transaction *transaction, Initiator initiator, double amount);
  void undoableSetDefaultValues(UNDO::Transaction *transaction, const PresetParameterGroup *values);

  GroupId getID() const;
  size_t getHash() const;

  virtual void copyFrom(UNDO::Transaction *transaction, const PresetParameterGroup *other);
  virtual void copyFrom(UNDO::Transaction *transaction, const ParameterGroup *other);

  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

  virtual tUpdateID onChange(uint64_t flags = UpdateDocumentContributor::ChangeFlags::Generic) override;

  // CALLBACKS
  sigc::connection onGroupChanged(const sigc::slot<void> &slot);

  VoiceGroup getVoiceGroup() const;

  void undoableLoadDefault(UNDO::Transaction *transaction, Defaults mode);

 protected:
  tParameterPtr appendParameter(Parameter *p);

 private:
  const GroupId m_id;
  const char *m_shortName;
  const char *m_longName;
  const char *m_webUIName;
  IntrusiveList<tParameterPtr> m_parameters;
  Signal<void> m_signalGroupChanged;
};
