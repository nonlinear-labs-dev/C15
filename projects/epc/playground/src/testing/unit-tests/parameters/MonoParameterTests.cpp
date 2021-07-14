#include <testing/TestHelper.h>
#include <catch.hpp>
#include "groups/MonoGroup.h"
#include "presets/EditBuffer.h"

TEST_CASE("Mono Parameter Helper")
{
  auto eb = TestHelper::getEditBuffer();
  for(auto vg : { VoiceGroup::I, VoiceGroup::II })
  {
    if(auto g = eb->getParameterGroupByID({ "Mono", vg }))
    {
      for(auto& p : g->getParameters())
      {
        CHECK(MonoGroup::isMonoParameter(p));
      }
    }
    else
    {
      CHECK(false);
    }
  }
}