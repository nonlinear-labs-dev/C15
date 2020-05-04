#include <testing/TestHelper.h>
#include <catch.hpp>

TEST_CASE("ParameterId hashes")
{
  SECTION("different numbers and voice groups have different hashes")
  {
    std::map<size_t, ParameterId> hashes;
    std::hash<ParameterId> hash;

    for(auto vg : { VoiceGroup::I, VoiceGroup::II, VoiceGroup::Global })
    {
      for(int i = 0; i < 400; i++)
      {
        ParameterId a(i, vg);
        auto h = hash(a);

        if(hashes.count(h))
        {
          auto &old = hashes.at(h);
          nltools::Log::warning("ParameterId", a.toString(), " collides with ", old.toString());
          CHECK(false);
        }
        hashes.insert(std::make_pair(h, std::move(a)));
      }
    }
  }
}
