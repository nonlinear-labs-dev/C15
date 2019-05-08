#pragma once

#include "playground.h"
#include <libundo/undo/Scope.h>

class EditBuffer;
class Parameter;

class EditBufferSnapshotMaker : public sigc::trackable
{
 public:
  static EditBufferSnapshotMaker &get();

  void addSnapshotIfRequired(UNDO::Scope::tTransactionPtr transaction);

 private:
  EditBufferSnapshotMaker();

  struct Record
  {
    Parameter *parameter;
    tControlPositionValue snapshotValue;
  };

  using tParams = list<Record>;

  tParams collectDirtyParameters(shared_ptr<EditBuffer> editBuffer);
  void addSnapshot(UNDO::Scope::tTransactionPtr transaction, tParams &&params);
  void sortParametersByModulationFlow(EditBufferSnapshotMaker::tParams &params);

  int m_lastKnownUpdateID = 0;
};
