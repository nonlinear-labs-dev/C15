#pragma once

#include "playground.h"
#include "groups/ParameterGroup.h"
#include <vector>
#include "tools/IntrusiveList.h"

class Parameter;

class ParameterGroupSet : public UpdateDocumentContributor
{
 public:
  ParameterGroupSet(UpdateDocumentContributor *parent);
  virtual ~ParameterGroupSet();

  virtual void init();

  typedef ParameterGroup *tParameterGroupPtr;

  size_t countParameters() const;
  tParameterGroupPtr getParameterGroupByID(const Glib::ustring &id) const;

  const IntrusiveList<tParameterGroupPtr> &getParameterGroups() const
  {
    return m_parameterGroups;
  }

  map<int, Parameter *> getParametersSortedById() const;
  Parameter *findParameterByID(size_t id) const;

  void writeDiff(Writer &writer, ParameterGroupSet *other) const;

 protected:
  void copyFrom(UNDO::Scope::tTransactionPtr transaction, ParameterGroupSet *other);
  virtual tParameterGroupPtr appendParameterGroup(ParameterGroup *p);

 private:
  IntrusiveList<tParameterGroupPtr> m_parameterGroups;
  map<int, Parameter *> m_idToParameterMap;
};
