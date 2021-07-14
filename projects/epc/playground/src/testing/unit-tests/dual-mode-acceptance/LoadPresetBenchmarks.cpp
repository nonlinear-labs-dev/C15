#include <utility>

#include "testing/TestHelper.h"
#include "testing/unit-tests/mock/MockPresetStorage.h"
#include "presets/PresetParameter.h"
#include "presets/Preset.h"

inline void preparePresets(MockPresetStorage& presets, double value)
{
  auto scope = TestHelper::createTestScope();
  presets.getLayerPreset()->forEachParameter([&](PresetParameter* p) { p->setValue(scope->getTransaction(), value); });

  presets.getSinglePreset()->forEachParameter([&](PresetParameter* p) { p->setValue(scope->getTransaction(), value); });

  presets.getSplitPreset()->forEachParameter([&](PresetParameter* p) { p->setValue(scope->getTransaction(), value); });
}

class Averager
{
 public:
  Averager(std::string name, size_t size)
      : m_name { std::move(name) }
  {
    m_values.reserve(size);
  }

  ~Averager()
  {
    auto total = std::accumulate(m_values.begin(), m_values.end(), 0ul);
    std::chrono::milliseconds totalMs
        = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::nanoseconds(total));
    nltools::Log::error(m_name, "took on avg:", totalMs.count() / m_values.size(), "ms");
  }

  void put(unsigned long val)
  {
    m_values.emplace_back(val);
  }

 private:
  std::vector<unsigned long> m_values;
  std::string m_name;
};

template <typename T> inline long measure(const T& cb)
{
  auto start = std::chrono::high_resolution_clock::now();
  cb();
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::nanoseconds(end - start).count();
}

TEST_CASE("Load Presets", "[Benchmark]")
{
  MockPresetStorage presets1;
  MockPresetStorage presets2;

  preparePresets(presets1, 0);
  preparePresets(presets2, 1.0);

  auto eb = TestHelper::getEditBuffer();
  EditBufferUseCases ebUseCases(eb);

  constexpr auto cNumTests = 10;

  Averager avgSingle("Single", cNumTests);
  Averager avgLayer("Layer", cNumTests);
  Averager avgSplit("Split", cNumTests);

  for(auto i = 0; i < cNumTests; i++)
  {

    avgSingle.put(measure([&]() {
      ebUseCases.undoableLoad(presets1.getSinglePreset());
      ebUseCases.undoableLoad(presets2.getSinglePreset());
    }));

    avgLayer.put(measure([&]() {
      ebUseCases.undoableLoad(presets1.getLayerPreset());
      ebUseCases.undoableLoad(presets2.getLayerPreset());
    }));

    avgSplit.put(measure([&] {
      ebUseCases.undoableLoad(presets1.getSplitPreset());
      ebUseCases.undoableLoad(presets2.getSplitPreset());
    }));
  }
}
