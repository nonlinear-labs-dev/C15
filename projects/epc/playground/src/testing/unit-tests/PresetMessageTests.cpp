#include <catch.hpp>
#include "proxies/audio-engine/AudioEngineProxy.h"
#include "presets/EditBuffer.h"
#include "testing/TestHelper.h"
#include "mock/MockPresetStorage.h"

template <typename T> void collectID(const T &t, std::unordered_map<int, int> &counts)
{
  try
  {
    auto &i = counts.at(t.id);
    i++;
  }
  catch(...)
  {
    counts[t.id] = 1;
  }
}

template <typename T> void collectIDs(const T &range, std::unordered_map<int, int> &counts)
{
  for(auto &t : range)
  {
    collectID(t, counts);
  }
}

void collectIDs(const nltools::msg::ParameterGroups::MonoGroup &group, std::unordered_map<int, int> &counts)
{
  collectID(group.legato, counts);
  collectID(group.priority, counts);
  collectID(group.monoEnable, counts);
  collectID(group.glide, counts);
}

void collectIDs(const nltools::msg::ParameterGroups::UnisonGroup &group, std::unordered_map<int, int> &counts)
{
  collectID(group.phase, counts);
  collectID(group.pan, counts);
  collectID(group.detune, counts);
  collectID(group.unisonVoices, counts);
}

template <typename T> void assertMap(const T &t)
{
  for(auto &idCount : t)
  {
    REQUIRE(idCount.second == 1);
  }
}

void assertNoIDTwice(const nltools::msg::SinglePresetMessage &msg)
{
  std::unordered_map<int, int> count;
  collectIDs(msg.unison, count);
  collectIDs(msg.mono, count);
  collectIDs(msg.unmodulateables, count);
  collectIDs(msg.modulateables, count);
  collectIDs(msg.hwamounts, count);
  collectIDs(msg.hwsources, count);
  collectIDs(msg.macros, count);
  collectIDs(msg.scale, count);
  assertMap(count);
}

template <int vg, typename tMsg> void collectDual(const tMsg &msg)
{
  std::unordered_map<int, int> count;
  collectIDs(msg.unison[vg], count);
  collectIDs(msg.mono[vg], count);
  collectIDs(msg.unmodulateables[vg], count);
  collectIDs(msg.modulateables[vg], count);
  collectID(msg.splitpoint[vg], count);
  collectIDs(msg.hwamounts, count);
  collectIDs(msg.hwsources, count);
  collectIDs(msg.macros, count);
  collectIDs(msg.scale, count);
  assertMap(count);
}

template <int vg> void collectDual(const nltools::msg::LayerPresetMessage &msg)
{
  std::unordered_map<int, int> count;
  collectIDs(msg.unison, count);
  collectIDs(msg.mono, count);
  collectIDs(msg.unmodulateables[vg], count);
  collectIDs(msg.modulateables[vg], count);
  collectIDs(msg.hwamounts, count);
  collectIDs(msg.hwsources, count);
  collectIDs(msg.macros, count);
  collectIDs(msg.scale, count);
  assertMap(count);
}

void assertNoIDTwice(const nltools::msg::SplitPresetMessage &msg)
{
  collectDual<0>(msg);
  collectDual<1>(msg);
}

void assertNoIDTwice(const nltools::msg::LayerPresetMessage &msg)
{
  collectDual<0>(msg);
  collectDual<1>(msg);
}

TEST_CASE("Single Preset Message")
{
  MockPresetStorage presets;
  auto editBuffer = TestHelper::getEditBuffer();

  SECTION("Single Message not duplicate IDs")
  {
    auto scope = TestHelper::createTestScope();
    editBuffer->undoableLoad(scope->getTransaction(), presets.getSinglePreset(), true);
    auto message = AudioEngineProxy::createSingleEditBufferMessage(*editBuffer);
    assertNoIDTwice(message);
  }

  SECTION("Split Message no duplicate IDs")
  {
    auto scope = TestHelper::createTestScope();
    editBuffer->undoableLoad(scope->getTransaction(), presets.getSplitPreset(), true);
    auto message = AudioEngineProxy::createSplitEditBufferMessage(*editBuffer);
    assertNoIDTwice(message);
  }

  SECTION("Layer Message no duplicate IDs")
  {
    auto scope = TestHelper::createTestScope();
    editBuffer->undoableLoad(scope->getTransaction(), presets.getLayerPreset(), true);
    auto message = AudioEngineProxy::createLayerEditBufferMessage(*editBuffer);
    assertNoIDTwice(message);
  }
}
