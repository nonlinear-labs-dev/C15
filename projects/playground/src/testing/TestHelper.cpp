#include "TestHelper.h"
#include "testing/unit-tests/mock/EditBufferNamedLogicalParts.h"

void TestHelper::randomizeCrossFBAndToFX(UNDO::Transaction* transaction)
{
  using EBL = EditBufferLogicalParts;

  for(auto p : EBL::getCrossFB<VoiceGroup::I>())
  {
    p->undoableRandomize(transaction, Initiator::EXPLICIT_OTHER, 1);
  }

  for(auto p : EBL::getToFX<VoiceGroup::I>())
  {
    p->undoableRandomize(transaction, Initiator::EXPLICIT_OTHER, 1);
  }

  for(auto p : EBL::getCrossFB<VoiceGroup::II>())
  {
    p->undoableRandomize(transaction, Initiator::EXPLICIT_OTHER, 1);
  }

  for(auto p : EBL::getToFX<VoiceGroup::II>())
  {
    p->undoableRandomize(transaction, Initiator::EXPLICIT_OTHER, 1);
  }
};

void TestHelper::randomizeFadeParams(UNDO::Transaction* transaction)
{
  using EBL = EditBufferLogicalParts;

  TestHelper::forceParameterChange(transaction, EBL::getFadeFrom<VoiceGroup::I>());
  TestHelper::forceParameterChange(transaction, EBL::getFadeFrom<VoiceGroup::II>());
  TestHelper::forceParameterChange(transaction, EBL::getFadeRange<VoiceGroup::I>());
  TestHelper::forceParameterChange(transaction, EBL::getFadeRange<VoiceGroup::II>());
};