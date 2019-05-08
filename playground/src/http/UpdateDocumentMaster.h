#pragma once

#include "playground.h"
#include "libundo/undo/Scope.h"
#include "UpdateDocumentContributor.h"

class Application;

class UpdateDocumentMaster : public UpdateDocumentContributor
{
 public:
  UpdateDocumentMaster();
  virtual ~UpdateDocumentMaster();

  tUpdateID onChange(uint64_t flags = UpdateDocumentContributor::ChangeFlags::Generic) override;
};
