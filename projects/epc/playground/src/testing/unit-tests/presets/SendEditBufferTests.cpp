#include "testing/TestHelper.h"
#include "presets/Preset.h"
#include "presets/PresetManager.h"
#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/Message.h>
#include <testing/unit-tests/mock/PreDualModePresetBank.h>
#include <testing/unit-tests/mock/MockPresetStorage.h>
#include <proxies/audio-engine/AudioEngineProxy.h>

struct ScopedMessagingConfiguration
{
  explicit ScopedMessagingConfiguration(const nltools::msg::Configuration &config)
  {
    m_oldConfig = nltools::msg::getConfig();
    nltools::msg::init(config);
  }

  ~ScopedMessagingConfiguration()
  {
    nltools::msg::init(m_oldConfig);
  }

  nltools::msg::Configuration m_oldConfig;
};

TEST_CASE("Preset Load sends EditBuffer")
{

  using namespace nltools::msg;
  using namespace std::chrono;

  Configuration configuration;
  configuration.useEndpoints = { EndPoint::AudioEngine };
  configuration.offerEndpoints = { EndPoint::Playground, EndPoint::AudioEngine };

  ScopedMessagingConfiguration scopeEndPoint { configuration };

  bool singleMessageRecieved = false;

  CHECK(waitForConnection(EndPoint::AudioEngine));
  auto c = receive<SinglePresetMessage>(EndPoint::AudioEngine,
                                        [&](const auto &singleEditMessage) { singleMessageRecieved = true; });

  auto eb = TestHelper::getEditBuffer();
  EditBufferUseCases ebUseCases(eb);

  MockPresetStorage presets;
  ebUseCases.undoableLoad(presets.getSinglePreset());

  TestHelper::doMainLoop(1s, 1s, [&] { return singleMessageRecieved; });
  c.disconnect();

  CHECK(eb->getUUIDOfLastLoadedPreset() == presets.getSinglePreset()->getUuid());
}

TEST_CASE("Store Action do not send EditBuffer")
{
  using namespace nltools::msg;
  using namespace std::chrono;

  Configuration configuration;
  configuration.useEndpoints = { EndPoint::AudioEngine };
  configuration.offerEndpoints = { EndPoint::Playground, EndPoint::AudioEngine };

  ScopedMessagingConfiguration scopeEndPoint { configuration };

  auto pm = TestHelper::getPresetManager();

  bool singleMessageReceived = false;

  {
    auto scope = TestHelper::createTestScope();
    TestHelper::initSingleEditBuffer(scope->getTransaction());
  }

  CHECK(waitForConnection(EndPoint::AudioEngine));
  auto c = receive<SinglePresetMessage>(EndPoint::AudioEngine,
                                        [&](const auto &singleEditMessage) { singleMessageReceived = true; });
  PresetManagerUseCases useCases(pm);

  //Store EditBuffer as new Bank
  auto oldNumBanks = pm->getNumBanks();
  useCases.createBankAndStoreEditBuffer();
  auto newNumBanks = pm->getNumBanks();
  TestHelper::doMainLoopIteration();
  CHECK(newNumBanks > oldNumBanks);

  auto bank = pm->getSelectedBank();

  //Append preset into bank
  auto oldNumPresets = bank->getNumPresets();
  useCases.appendPreset(bank);
  auto newNumPresets = bank->getNumPresets();
  CHECK(newNumPresets > oldNumPresets);

  //Insert preset into bank at pos 0
  oldNumPresets = bank->getNumPresets();
  useCases.insertPreset(bank, 0);
  newNumPresets = bank->getNumPresets();
  CHECK(newNumPresets > oldNumPresets);

  TestHelper::doMainLoopIteration();
  //No preset message was send!
  CHECK(!singleMessageReceived);
  c.disconnect();
}