#pragma once

#include "UpdateDocumentContributor.h"

class UpdateDocumentMaster : public UpdateDocumentContributor
{
 public:
  UpdateDocumentMaster();
  ~UpdateDocumentMaster() override;

  tUpdateID onChange(uint64_t flags = UpdateDocumentContributor::ChangeFlags::Generic) override;
  uint64_t getAndResetFlags();

 private:
  uint64_t m_flags = 0;
};
