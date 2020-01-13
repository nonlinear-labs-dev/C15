#include <groups/HardwareSourcesGroup.h>
#include <groups/MacroControlMappingGroup.h>
#include <groups/MacroControlsGroup.h>
#include <groups/ScaleGroup.h>
#include <groups/MasterGroup.h>
#include <groups/MonoGroup.h>
#include <groups/UnisonGroup.h>
#include "third-party/include/catch.hpp"
#include "testing/TestHelper.h"
#include "presets/EditBuffer.h"

namespace GroupAffiliation
{
  inline bool isRealGlobal(const Parameter* parameter)
  {
    auto parent = parameter->getParent();
    return dynamic_cast<HardwareSourcesGroup*>(parent) || dynamic_cast<MacroControlMappingGroup*>(parent)
        || dynamic_cast<MacroControlsGroup*>(parent) || dynamic_cast<ScaleGroup*>(parent)
        || dynamic_cast<MasterGroup*>(parent);
  }
}

TEST_CASE("Parameter Dual/Global affiliation - Single-Sound")
{
  TestHelper::forEachParameter(
      [](const Parameter* parameter) {
        REQUIRE_FALSE(EditBuffer::isDualParameterForSoundType(parameter, SoundType::Single));
      },
      TestHelper::getEditBuffer());
}

TEST_CASE("Parameter Dual/Global affiliation - Split Sound")
{
  TestHelper::forEachParameter(
      [](const Parameter* parameter) {
        auto ret = EditBuffer::isDualParameterForSoundType(parameter, SoundType::Split);

        if(GroupAffiliation::isRealGlobal(parameter))
          REQUIRE_FALSE(ret);
        else
          REQUIRE(ret);
      },
      TestHelper::getEditBuffer());
}

TEST_CASE("Parameter Dual/Global affiliation - Layer Sound")
{
  TestHelper::forEachParameter(
      [](const Parameter* parameter) {
        auto isDual = EditBuffer::isDualParameterForSoundType(parameter, SoundType::Layer);

        if(GroupAffiliation::isRealGlobal(parameter))
        {
          REQUIRE_FALSE(isDual);
        }
        else
        {
          if(MonoGroup::isMonoParameter(parameter))
          {
            REQUIRE_FALSE(isDual);
          }
          else if(UnisonGroup::isUnisonParameter(parameter))
          {
            REQUIRE_FALSE(isDual);
          }
          else
          {
            REQUIRE(isDual);
          }
        }
      },
      TestHelper::getEditBuffer());
}