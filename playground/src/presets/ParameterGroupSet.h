#pragma once

#include "playground.h"
#include "groups/ParameterGroup.h"
#include <presets/AttributesOwner.h>
#include <vector>
#include "tools/IntrusiveList.h"

class Parameter;
class Preset;

class ParameterGroupSet : public AttributesOwner
{
 private:
  using super = AttributesOwner;

 public:
  ParameterGroupSet(UpdateDocumentContributor *parent);
  ~ParameterGroupSet() override;

  virtual void init();

  typedef ParameterGroup *tParameterGroupPtr;

  size_t countParameters() const;
  tParameterGroupPtr getParameterGroupByID(const Glib::ustring &id) const;

  const IntrusiveList<tParameterGroupPtr> &getParameterGroups() const
  {
    return m_parameterGroups;
  }

  map<int, Parameter *> getParametersSortedById() const;
  Parameter *findParameterByID(int id) const;

  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

  template<typename T>
  void forEachParameter(T t) {
      for(auto& g: m_parameterGroups) {
          for(auto& p: g->getParameters()) {
              t(p);
          }
      }
  }

 protected:
  void copyFrom(UNDO::Transaction *transaction, const Preset *other);
  virtual tParameterGroupPtr appendParameterGroup(ParameterGroup *p);

 private:
  IntrusiveList<tParameterGroupPtr> m_parameterGroups;
  map<int, Parameter *> m_idToParameterMap;
};
