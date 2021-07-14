#include "../TestHelper.h"
#include <catch.hpp>
#include "presets/EditBuffer.h"
#include <groups/ParameterGroup.h>

namespace parameterdetail
{
  template <typename tCB> void forAllParametersInAllVoiceGroupsDo(const tCB &cb)
  {
    auto editBuffer = TestHelper::getEditBuffer();

    for(auto vg : { VoiceGroup::I, VoiceGroup::II, VoiceGroup::Global })
    {
      for(auto &g : editBuffer->getParameterGroups(vg))
      {
        for(auto &p : g->getParameters())
        {
          cb(p);
        }
      }
    }
  }
}

TEST_CASE("Parameters do not Crash")
{
  SECTION("Select Parameter")
  {
    auto editBuffer = TestHelper::getEditBuffer();
    parameterdetail::forAllParametersInAllVoiceGroupsDo([&](const Parameter *p) {
      auto scope = TestHelper::createTestScope();
      REQUIRE_NOTHROW(editBuffer->undoableSelectParameter(scope->getTransaction(), p->getID(), false));
      REQUIRE(editBuffer->getSelectedParameterNumber() == p->getID().getNumber());
    });
  }
}

TEST_CASE("Parameter Groups are conforming to GroupID rules")
{
  for(auto vg : { VoiceGroup::I, VoiceGroup::II })
  {
    for(auto &group : TestHelper::getEditBuffer()->getParameterGroups(vg))
    {
      REQUIRE_FALSE(GroupId::isGlobal(group->getID().getName()));
      for(auto &p : group->getParameters())
        REQUIRE_FALSE(ParameterId::isGlobal(p->getID().getNumber()));
    }
  }

  for(auto &group : TestHelper::getEditBuffer()->getParameterGroups(VoiceGroup::Global))
  {
    REQUIRE(GroupId::isGlobal(group->getID().getName()));
    for(auto &p : group->getParameters())
      REQUIRE(ParameterId::isGlobal(p->getID().getNumber()));
  }
}
