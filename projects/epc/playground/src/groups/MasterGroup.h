#pragma once

#include "ParameterGroup.h"

class MasterGroup : public ParameterGroup
{
 public:
  explicit MasterGroup(ParameterGroupSet *parent);
  ~MasterGroup() override;

  void init() override;
  void undoableRandomize(UNDO::Transaction *transaction, Initiator initiator, double amount) override;
};
