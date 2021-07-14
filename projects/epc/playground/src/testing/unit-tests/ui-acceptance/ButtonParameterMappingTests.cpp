#include <catch.hpp>
#include "proxies/hwui/panel-unit/ButtonParameterMapping.h"
#include <set>

TEST_CASE("No parameter ID duplicates in mapping")
{
  ButtonParameterMapping mapping;

  std::set<int> seenIds {};

  mapping.forEachButton([&](auto button, auto ids) {
    for(auto id : ids)
    {
      auto ret = seenIds.insert(id);
      REQUIRE(ret.second);
    }
  });
}