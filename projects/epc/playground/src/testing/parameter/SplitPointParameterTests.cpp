#include <testing/TestHelper.h>
#include <parameter_declarations.h>
#include <Application.h>
#include <device-settings/SplitPointSyncParameters.h>
#include <device-settings/Settings.h>
#include <parameters/SplitPointParameter.h>
#include <nltools/messaging/Message.h>
#include <nltools/messaging/Messaging.h>
#include <device-settings/SyncSplitSettingUseCases.h>

TEST_CASE("Split Point Unlinked with Overlap gets linked -> msg is correct")
{
  auto eb = TestHelper::getEditBuffer();
  auto splitI = eb->findAndCastParameterByID<SplitPointParameter>({ C15::PID::Split_Split_Point, VoiceGroup::I });
  auto splitII = eb->findAndCastParameterByID<SplitPointParameter>({ C15::PID::Split_Split_Point, VoiceGroup::II });
  auto useCases = SyncSplitSettingUseCases::get();

  useCases.disableSyncSetting();

  WHEN("Split I and II have Overlap")
  {
    {
      auto scope = TestHelper::createTestScope();
      splitI->setCPFromHwui(scope->getTransaction(), 0.6);
      splitII->setCPFromHwui(scope->getTransaction(), 0.5);
    }

    CHECK(splitI->hasOverlap());
    CHECK(splitII->hasOverlap());
  }

  THEN("Link")
  {
    useCases.enableSyncSetting();

    CHECK(!splitI->hasOverlap());
    CHECK(!splitII->hasOverlap());
  }
}
