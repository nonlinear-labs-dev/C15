#include "testing/TestHelper.h"
#include <catch.hpp>
#include "presets/EditBuffer.h"
#include <groups/ParameterGroup.h>

TEST_CASE("No Parameter Name Contains '@VG' placeholder")
{
  for(auto vg : { VoiceGroup::I, VoiceGroup::II, VoiceGroup::Global })
    for(auto& g : TestHelper::getEditBuffer()->getParameterGroups(vg))
      for(auto& p : g->getParameters())
      {
        CHECK(p->getLongName().find("@VG") == std::string::npos);
        CHECK(p->getShortName().find("@VG") == std::string::npos);
      }
}
