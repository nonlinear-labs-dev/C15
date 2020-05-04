#include <catch.hpp>
#include <parameters/UnisonVoicesParameter.h>
#include "testing/TestHelper.h"

TEST_CASE("Convert Single to Dual Sound Changes Unison Accordingly", "[Unison][Parameter]")
{
  auto eb = TestHelper::getEditBuffer();
  auto scope = TestHelper::createTestScope();
  auto transaction = scope->getTransaction();
  TestHelper::initSingleEditBuffer(transaction);

  auto unisonVoices = eb->findParameterByID({ 249, VoiceGroup::I });
  unisonVoices->setCPFromHwui(scope->getTransaction(), 0);
  REQUIRE(unisonVoices->getDisplayString() == "1 voice (off)");

  WHEN("Unison Voices: 1")
  {
    unisonVoices->setCPFromHwui(transaction, 0);  // -> 1
    REQUIRE(unisonVoices->getDisplayString() == "1 voice (off)");

    WHEN("Converted to Dual")
    {
      eb->undoableConvertToDual(transaction, SoundType::Layer);
      REQUIRE(unisonVoices->getDisplayString() == "1 voice (off)");
    }
  }

  for(auto steps = 1; steps < 24; steps++)
  {
    auto voices = steps + 1;

    WHEN("Unison Voices: " + std::to_string(voices))
    {
      unisonVoices->setCPFromHwui(transaction, 0);
      unisonVoices->stepCPFromHwui(transaction, steps, {});
      REQUIRE(unisonVoices->getDisplayString() == std::to_string(voices) + " voices");

      eb->undoableConvertToDual(transaction, SoundType::Layer);

      if(voices >= 12)
      {
        CHECK(unisonVoices->getDisplayString() == "12 voices");
      }
      else
      {
        CHECK(unisonVoices->getDisplayString() == std::to_string(voices) + " voices");
      }
    }
  }
}

TEST_CASE("Convert Dual to Single Sound", "[Unison][Parameter]")
{
  auto eb = TestHelper::getEditBuffer();
  auto scope = TestHelper::createTestScope();
  auto transaction = scope->getTransaction();

  eb->undoableConvertToDual(transaction, SoundType::Layer);
  eb->undoableInitSound(transaction);

  auto unisonVoices = eb->findParameterByID({ 249, VoiceGroup::I });
  unisonVoices->setCPFromHwui(scope->getTransaction(), 0);
  REQUIRE(unisonVoices->getDisplayString() == "1 voice (off)");

  WHEN("Unison Voices: 1")
  {
    unisonVoices->setCPFromHwui(transaction, 0);  // -> 1
    REQUIRE(unisonVoices->getDisplayString() == "1 voice (off)");

    WHEN("Converted to Single")
    {
      eb->undoableConvertToSingle(transaction, VoiceGroup::I);

      CHECK(unisonVoices->getDisplayString() == "1 voice (off)");
    }
  }

  for(auto steps = 1; steps < 12; steps++)
  {
    auto voices = steps + 1;

    WHEN("Unison Voices: " + std::to_string(voices))
    {
      unisonVoices->setCPFromHwui(transaction, 0);
      unisonVoices->stepCPFromHwui(transaction, steps, {});
      REQUIRE(unisonVoices->getDisplayString() == std::to_string(voices) + " voices");

      eb->undoableConvertToSingle(transaction, VoiceGroup::I);

      CHECK(unisonVoices->getDisplayString() == std::to_string(voices) + " voices");
    }
  }
}

TEST_CASE("Undo Convert Sound resets Scaling", "[Unison][Parameter]")
{
  auto eb = TestHelper::getEditBuffer();
  auto unisonVoicesI = eb->findParameterByID({ 249, VoiceGroup::I });
  auto unisonVoicesII = eb->findParameterByID({ 249, VoiceGroup::II });

  SECTION("Single -> Dual")
  {
    //Init Single
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::I);

      REQUIRE(eb->getType() == SoundType::Single);
      REQUIRE(unisonVoicesI->getValue().getCoarseDenominator() == 23);
      REQUIRE(unisonVoicesII->getValue().getCoarseDenominator() == 23);
    }

    //convert to dual
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Layer);

      REQUIRE(eb->getType() == SoundType::Layer);
      REQUIRE(unisonVoicesI->getValue().getCoarseDenominator() == 11);
      REQUIRE(unisonVoicesII->getValue().getCoarseDenominator() == 11);

      unisonVoicesI->setCPFromHwui(scope->getTransaction(), 1);
      unisonVoicesII->setCPFromHwui(scope->getTransaction(), 1);
      REQUIRE(unisonVoicesI->getDisplayString() == "12 voices");
      REQUIRE(unisonVoicesII->getDisplayString() == "12 voices");

      unisonVoicesI->setCPFromHwui(scope->getTransaction(), 0);
      unisonVoicesII->setCPFromHwui(scope->getTransaction(), 0);
      REQUIRE(unisonVoicesI->getDisplayString() == "1 voice (off)");
      REQUIRE(unisonVoicesII->getDisplayString() == "1 voice (off)");
    }

    //undo
    {
      REQUIRE(eb->getType() == SoundType::Layer);
      eb->getParent()->getUndoScope().undo();
      REQUIRE(eb->getType() == SoundType::Single);

      REQUIRE(unisonVoicesI->getValue().getCoarseDenominator() == 23);
      REQUIRE(unisonVoicesII->getValue().getCoarseDenominator() == 23);

      {
        auto scope = TestHelper::createTestScope();
        unisonVoicesI->setCPFromHwui(scope->getTransaction(), 1);
        unisonVoicesII->setCPFromHwui(scope->getTransaction(), 1);
        REQUIRE(unisonVoicesI->getDisplayString() == "24 voices");
        REQUIRE(unisonVoicesII->getDisplayString() == "24 voices");

        unisonVoicesI->setCPFromHwui(scope->getTransaction(), 0);
        unisonVoicesII->setCPFromHwui(scope->getTransaction(), 0);
        REQUIRE(unisonVoicesI->getDisplayString() == "1 voice (off)");
        REQUIRE(unisonVoicesII->getDisplayString() == "1 voice (off)");
      }
    }
  }

  SECTION("Dual -> Single")
  {
    //Init dual
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToDual(scope->getTransaction(), SoundType::Layer);

      REQUIRE(eb->getType() == SoundType::Layer);
      REQUIRE(unisonVoicesI->getValue().getCoarseDenominator() == 11);
      REQUIRE(unisonVoicesII->getValue().getCoarseDenominator() == 11);
    }

    //convert to single
    {
      auto scope = TestHelper::createTestScope();
      eb->undoableConvertToSingle(scope->getTransaction(), VoiceGroup::I);

      REQUIRE(eb->getType() == SoundType::Single);
      REQUIRE(unisonVoicesI->getValue().getCoarseDenominator() == 23);
      REQUIRE(unisonVoicesII->getValue().getCoarseDenominator() == 23);

      unisonVoicesI->setCPFromHwui(scope->getTransaction(), 1);
      unisonVoicesII->setCPFromHwui(scope->getTransaction(), 1);
      REQUIRE(unisonVoicesI->getDisplayString() == "24 voices");
      REQUIRE(unisonVoicesII->getDisplayString() == "24 voices");

      unisonVoicesI->setCPFromHwui(scope->getTransaction(), 0);
      unisonVoicesII->setCPFromHwui(scope->getTransaction(), 0);
      REQUIRE(unisonVoicesI->getDisplayString() == "1 voice (off)");
      REQUIRE(unisonVoicesII->getDisplayString() == "1 voice (off)");
    }

    //undo
    {
      REQUIRE(eb->getType() == SoundType::Single);
      eb->getParent()->getUndoScope().undo();
      REQUIRE(eb->getType() == SoundType::Layer);

      REQUIRE(unisonVoicesI->getValue().getCoarseDenominator() == 11);
      REQUIRE(unisonVoicesII->getValue().getCoarseDenominator() == 11);

      {
        auto scope = TestHelper::createTestScope();
        unisonVoicesI->setCPFromHwui(scope->getTransaction(), 1);
        unisonVoicesII->setCPFromHwui(scope->getTransaction(), 1);
        REQUIRE(unisonVoicesI->getDisplayString() == "12 voices");
        REQUIRE(unisonVoicesII->getDisplayString() == "12 voices");

        unisonVoicesI->setCPFromHwui(scope->getTransaction(), 0);
        unisonVoicesII->setCPFromHwui(scope->getTransaction(), 0);
        REQUIRE(unisonVoicesI->getDisplayString() == "1 voice (off)");
        REQUIRE(unisonVoicesII->getDisplayString() == "1 voice (off)");
      }
    }
  }
}