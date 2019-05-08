#pragma once

#include "ParameterGroup.h"

class MasterGroup : public ParameterGroup
{
 public:
  MasterGroup(ParameterGroupSet *parent);
  virtual ~MasterGroup();

  void init();

  virtual void undoableRandomize(UNDO::Scope::tTransactionPtr transaction, Initiator initiator, double amount) override;
};
