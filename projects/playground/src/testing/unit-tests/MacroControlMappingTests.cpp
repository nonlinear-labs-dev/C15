#include <catch.hpp>
#include "testing/TestHelper.h"
#include "groups/MacroControlMappingGroup.h"
#include "parameters/ModulationRoutingParameter.h"
#include "parameters/MacroControlParameter.h"
#include "presets/EditBuffer.h"

TEST_CASE("Macro Control Mapping initialized")
{
  auto eb = TestHelper::getEditBuffer();
  auto mcm = eb->getParameterGroupByID({ "MCM", VoiceGroup::Global });
  WHEN("Mappings have MC as targets")
  {
    for(auto &m : mcm->getParameters())
    {
      if(auto mappingParam = dynamic_cast<ModulationRoutingParameter *>(m))
      {
        REQUIRE(dynamic_cast<MacroControlParameter *>(mappingParam->getTargetParameter()) != nullptr);
      }
    }
  }
}
