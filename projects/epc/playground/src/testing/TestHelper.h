#pragma once
#include <libundo/undo/TransactionCreationScope.h>
#include <memory>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/Parameter.h>
#include <catch.hpp>
#include <libundo/undo/Scope.h>
#include <Application.h>
#include <parameters/ModulateableParameter.h>
#include <nltools/messaging/Message.h>

namespace TestHelper
{
  inline nltools::msg::Setting::MidiSettingsMessage::tRoutingMappings createFullMappings(bool state)
  {
    typedef nltools::msg::Setting::MidiSettingsMessage::tRoutingMappings tHW;
    tHW ret;
    for(auto& row: ret) {
      row.fill(state);
    }
    return ret;
  }

  namespace floating
  {
    template <typename T> inline bool equals(T first, T second)
    {
      constexpr auto epsilon = std::numeric_limits<T>::epsilon();
      return std::abs(first - second) <= epsilon;
    }

    template <typename T> inline bool differs(T first, T second)
    {
      return !equals(first, second);
    }
  }

  inline PresetManager* getPresetManager()
  {
    return Application::get().getPresetManager();
  }

  inline EditBuffer* getEditBuffer()
  {
    return getPresetManager()->getEditBuffer();
  }

  inline std::unique_ptr<UNDO::TransactionCreationScope> createTestScope()
  {
    return std::move(getPresetManager()->getUndoScope().startTestTransaction());
  }

  template <SoundType tType> inline void initDualEditBuffer(UNDO::Transaction* transaction)
  {
    auto eb = getEditBuffer();
    eb->undoableUnlockAllGroups(transaction);
    eb->undoableConvertToDual(transaction, tType);
    eb->undoableInitSound(transaction, Defaults::FactoryDefault);
  }

  template <SoundType tType> inline void initDualEditBuffer()
  {
    auto scope = UNDO::Scope::startTrashTransaction();
    initDualEditBuffer<tType>(scope->getTransaction());
  }

  inline void initSingleEditBuffer(UNDO::Transaction* transaction)
  {
    auto eb = getEditBuffer();
    eb->undoableUnlockAllGroups(transaction);
    eb->undoableConvertToSingle(transaction, VoiceGroup::I);
    eb->undoableInitSound(transaction, Defaults::FactoryDefault);
  }

  inline void forceParameterChange(UNDO::Transaction* transaction, Parameter* param)
  {
    auto currentValue = param->getControlPositionValue();

    auto incNext = param->getValue().getNextStepValue(1, {});
    auto decNext = param->getValue().getNextStepValue(-1, {});

    if(floating::differs(incNext, currentValue))
      param->setCPFromHwui(transaction, incNext);
    else if(floating::differs(decNext, currentValue))
      param->setCPFromHwui(transaction, decNext);
    else
      nltools_detailedAssertAlways(false, "Unable to change Parameter Value in either direction");
  }

  template <typename tCB> inline void forEachParameter(const tCB& cb, EditBuffer* eb)
  {
    for(auto vg : { VoiceGroup::I, VoiceGroup::II, VoiceGroup::Global })
      for(auto& g : eb->getParameterGroups(vg))
        for(auto& p : g->getParameters())
          cb(p);
  }

  inline void changeAllParameters(UNDO::Transaction* transaction)
  {
    auto eb = TestHelper::getEditBuffer();
    eb->forEachParameter([&](Parameter* param) { TestHelper::forceParameterChange(transaction, param); });
  };

  void randomizeCrossFBAndToFX(UNDO::Transaction* transaction);

  void randomizeFadeParams(UNDO::Transaction* transaction);

  inline void doMainLoop(std::chrono::milliseconds minTime, std::chrono::milliseconds timeout,
                         const std::function<bool()>& test)
  {
    Expiration exp;
    exp.refresh(timeout);

    Expiration min;

    if(minTime != std::chrono::milliseconds::zero())
      min.refresh(minTime);

    while((exp.isPending() && !test()) || min.isPending())
      g_main_context_iteration(nullptr, TRUE);

    CHECK(test());
  }

  inline void doMainLoopIteration()
  {
    g_main_context_iteration(nullptr, TRUE);
  }

  inline void updateMappings(nltools::msg::Setting::MidiSettingsMessage::tRoutingMappings& array,
                      nltools::msg::Setting::MidiSettingsMessage::RoutingAspect index, bool b)
  {
    for(auto& hw: array) {
      hw[static_cast<int>(index)] = b;
    }
  }

  inline void updateMappingForRow(nltools::msg::Setting::MidiSettingsMessage::tRoutingMappings& array,
                                  nltools::msg::Setting::MidiSettingsMessage::RoutingIndex index,
                                  bool value)
  {
    for(auto& a: array[static_cast<int>(index)])
    {
      a = value;
    }
  }

  inline void updateMappingForHW(nltools::msg::Setting::MidiSettingsMessage::tRoutingMappings& array,
                                 nltools::msg::Setting::MidiSettingsMessage::RoutingIndex index,
                                 nltools::msg::Setting::MidiSettingsMessage::RoutingAspect aspect, bool b)
  {
    array[static_cast<int>(index)][static_cast<int>(aspect)] = b;
  }
}

inline std::pair<double, double> getNextStepValuesFromValue(Parameter* p, double v)
{
  auto scope = UNDO::Scope::startTrashTransaction();
  auto oldCP = p->getControlPositionValue();
  p->setCPFromHwui(scope->getTransaction(), v);
  auto ret
      = std::make_pair<double, double>(p->getValue().getNextStepValue(-1, {}), p->getValue().getNextStepValue(1, {}));
  p->setCPFromHwui(scope->getTransaction(), oldCP);
  return ret;
}

#define CHECK_PARAMETER_CP_EQUALS_FICTION(p, v)                                                                        \
  {                                                                                                                    \
    auto range = getNextStepValuesFromValue(p, v);                                                                     \
    auto inRange = p->getControlPositionValue() >= range.first && p->getControlPositionValue() <= range.second;        \
    if(!inRange)                                                                                                       \
    {                                                                                                                  \
      nltools::Log::error(p->getLongName(), '(', p->getID().toString(), ") got", p->getControlPositionValue(),         \
                          "expected ~", v);                                                                            \
      CHECK(inRange);                                                                                                  \
      return;                                                                                                          \
    }                                                                                                                  \
    CHECK(p->getControlPositionValue() >= range.first);                                                                \
    CHECK(p->getControlPositionValue() <= range.second);                                                               \
  }

#define CHECK_PARAMETER_CP_UNEQUALS_FICTION(p, v)                                                                      \
  {                                                                                                                    \
    auto range = getNextStepValuesFromValue(p, v);                                                                     \
    auto inRange = p->getControlPositionValue() >= range.first && p->getControlPositionValue() <= range.second;        \
    if(inRange)                                                                                                        \
    {                                                                                                                  \
      nltools::Log::error(p->getLongName(), '(', p->getID().toString(), ") got", p->getControlPositionValue(),         \
                          "expected unequals ~", v);                                                                   \
      CHECK(false);                                                                                                    \
      return;                                                                                                          \
    }                                                                                                                  \
    CHECK_FALSE(p->getControlPositionValue() >= range.first);                                                          \
    CHECK_FALSE(p->getControlPositionValue() <= range.second);                                                         \
  }
