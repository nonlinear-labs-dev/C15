#include "SplitPointParameter.h"
#include <parameters/scale-converters/LinearBipolar60StScaleConverter.h>
#include <parameters/scale-converters/SplitPointScaleConverter.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ModulateableDualVoiceGroupMasterAndSplitPointLayout.h>
#include "groups/ParameterGroup.h"
#include "proxies/hwui/HWUI.h"
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterInfoLayout.h>
#include <parameters/scale-converters/dimension/SplitPointDimension.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <device-settings/Settings.h>
#include <device-settings/SplitPointSyncParameters.h>
#include <parameter_declarations.h>
#include <libundo/undo/Scope.h>

SplitPointParameter::SplitPointParameter(ParameterGroup* group, const ParameterId& id)
    : ModulateableParameterWithUnusualModUnit(group, id, ScaleConverter::get<SplitPointScaleConverter>(),
                                              ScaleConverter::get<LinearBipolar60StScaleConverter>(),
                                              id.getVoiceGroup() == VoiceGroup::I ? 0.5 : 0.516666667, 60, 60)
{
  auto sync = Application::get().getSettings()->getSetting<SplitPointSyncParameters>();

  sync->onChange(sigc::mem_fun(this, &SplitPointParameter::onSyncSettingChanged));
  sync->onChangeWithTransaction(sigc::mem_fun(this, &SplitPointParameter::onSyncSettingChangedWithTransaction));
  sync->onPresetStored(sigc::mem_fun(this, &SplitPointParameter::onStoreHappened));
}

void SplitPointParameter::onSyncSettingChanged(const Setting* s)
{
  onSyncSettingChangedWithTransaction(s, nullptr);
}

void SplitPointParameter::onSyncSettingChangedWithTransaction(const Setting* s, UNDO::Transaction* transaction)
{
  if(!m_settingGuard.isLocked())
  {
    if(getVoiceGroup() == VoiceGroup::I)
    {
      if(auto sync = dynamic_cast<const SplitPointSyncParameters*>(s))
      {
        if(sync->get())
        {
          std::unique_ptr<UNDO::TransactionCreationScope> scope = nullptr;

          if(transaction == nullptr)
          {
            scope = getUndoScope().startTransaction("Enable Split Sync");
            transaction = scope->getTransaction();
          }

          auto myCP = getControlPositionValue();
          getSibling()->setCpValue(transaction, Initiator::INDIRECT_SPLIT_SYNC,
                                   getValue().getNextStepValue(myCP, 1, {}), true);
          transaction->addSimpleCommand(
              [this](UNDO::Transaction::State s) {
                if(s == UNDO::Transaction::State::REDOING)
                {
                  auto l = m_settingGuard.lock();
                  Application::get().getSettings()->getSetting<SplitPointSyncParameters>()->setState(true);
                }
              },
              [this](UNDO::Transaction::State) {
                auto l = m_settingGuard.lock();
                Application::get().getSettings()->getSetting<SplitPointSyncParameters>()->setState(false);
              });
        }
      }
    }
  }
}

void SplitPointParameter::onStoreHappened(UNDO::Transaction* transaction, Preset* newPreset)
{
  //disable sync when no overlap is present and some preset got stored
  if(getVoiceGroup() == VoiceGroup::I)
  {
    auto setting = Application::get().getSettings()->getSetting<SplitPointSyncParameters>();
    //no Overlap
    if(!hasOverlap())
    {
      const auto oldState = setting->get();

      {
        auto l = m_settingGuard.lock();
        setting->setState(true);
      }

      transaction->addSimpleCommand(
          [this](UNDO::Transaction::State s) {
            if(s == UNDO::Transaction::State::REDOING)
            {
              auto l = m_settingGuard.lock();
              Application::get().getSettings()->getSetting<SplitPointSyncParameters>()->setState(true);
            }
          },
          [this, oldState](UNDO::Transaction::State) {
            auto l = m_settingGuard.lock();
            Application::get().getSettings()->getSetting<SplitPointSyncParameters>()->setState(oldState);
          });
    }
  }
}

Layout* SplitPointParameter::createLayout(FocusAndMode focusAndMode) const
{
  switch(focusAndMode.mode)
  {
    case UIMode::Info:
      return new ParameterInfoLayout();
    case UIMode::Edit:
      return new SplitPointParameterEditLayout();
    case UIMode::Select:
    default:
      return new SplitPointParameterLayout();
  }
}

void SplitPointParameter::setCpValue(UNDO::Transaction* transaction, Initiator initiator, tControlPositionValue value,
                                     bool dosendToPlaycontroller)
{
  const auto syncActive = Application::get().getSettings()->getSetting<SplitPointSyncParameters>()->get();
  if(syncActive && isAtExtremes(value) && initiator != Initiator::INDIRECT_SPLIT_SYNC)
  {
    clampToExtremes(transaction, dosendToPlaycontroller);
    return;
  }

  Parameter::setCpValue(transaction, initiator, value, dosendToPlaycontroller);

  if(syncActive)
  {
    if(initiator != Initiator::INDIRECT_SPLIT_SYNC)
    {
      auto other = getSibling();
      auto siblingValue = getValue().getNextStepValue(value, other->getVoiceGroup() == VoiceGroup::I ? -1 : 1, {});
      other->setCpValue(transaction, Initiator::INDIRECT_SPLIT_SYNC, siblingValue, dosendToPlaycontroller);
    }
  }
  else
  {
    preventNegativeOverlap(transaction, value, dosendToPlaycontroller);
  }
}

void SplitPointParameter::clampToExtremes(UNDO::Transaction* transaction, bool dosendToPlaycontroller)
{
  auto other = getSibling();
  if(getVoiceGroup() == VoiceGroup::I)
  {
    auto myVal = getValue().getNextStepValue(1, -1, {});
    auto otherVal = 1.0;
    other->setCpValue(transaction, Initiator::INDIRECT_SPLIT_SYNC, otherVal, dosendToPlaycontroller);
    setCpValue(transaction, Initiator::INDIRECT_SPLIT_SYNC, myVal, dosendToPlaycontroller);
  }
  else if(getVoiceGroup() == VoiceGroup::II)
  {
    auto myVal = getValue().getNextStepValue(0, 1, {});
    auto otherVal = 0.0;
    other->setCpValue(transaction, Initiator::INDIRECT_SPLIT_SYNC, otherVal, dosendToPlaycontroller);
    setCpValue(transaction, Initiator::INDIRECT_SPLIT_SYNC, myVal, dosendToPlaycontroller);
  }
}

void SplitPointParameter::preventNegativeOverlap(UNDO::Transaction* transaction, tControlPositionValue value,
                                                 bool dosendToPlaycontroller)
{
  auto sibling = getSibling();
  auto siblingValue = sibling->getControlPositionValue();
  auto inc = sibling->getVoiceGroup() == VoiceGroup::I ? 1 : -1;

  auto siblingThreshold = getValue().getNextStepValue(siblingValue, inc, {});
  siblingValue = getValue().getNextStepValue(value, -inc, {});

  if(getVoiceGroup() == VoiceGroup::I)
  {
    if(value < siblingThreshold)
    {
      sibling->setCpValue(transaction, Initiator::INDIRECT_SPLIT_SYNC, siblingValue, dosendToPlaycontroller);
    }
  }
  else
  {
    if(value > siblingThreshold)
    {
      sibling->setCpValue(transaction, Initiator::INDIRECT_SPLIT_SYNC, siblingValue, dosendToPlaycontroller);
    }
  }
}

void SplitPointParameter::setModulationAmount(UNDO::Transaction* transaction, const tDisplayValue& amount)
{
  ModulateableParameter::setModulationAmount(transaction, amount);
  getSibling()->setModulationAmountFromSibling(transaction, amount);
}

void SplitPointParameter::setModulationSource(UNDO::Transaction* transaction, MacroControls src)
{
  ModulateableParameter::setModulationSource(transaction, src);
  getSibling()->setModulationSourceFromSibling(transaction, src);
}

Glib::ustring SplitPointParameter::stringizeModulationAmount(tControlPositionValue amount) const
{
  return std::to_string(static_cast<int>(60 * amount)) + " st";
}

SplitPointParameter* SplitPointParameter::getSibling() const
{
  return static_cast<SplitPointParameter*>(Application::get().getPresetManager()->getEditBuffer()->findParameterByID(
      { C15::PID::Split_Split_Point, getVoiceGroup() == VoiceGroup::I ? VoiceGroup::II : VoiceGroup::I }));
}

void SplitPointParameter::setModulationAmountFromSibling(UNDO::Transaction* transaction, const tDisplayValue& amount)
{
  ModulateableParameter::setModulationAmount(transaction, amount);
}

void SplitPointParameter::setModulationSourceFromSibling(UNDO::Transaction* transaction, MacroControls src)
{
  ModulateableParameter::setModulationSource(transaction, src);
}

bool SplitPointParameter::isAtExtremes(tControlPositionValue value)
{
  if(getVoiceGroup() == VoiceGroup::I)
  {
    return value > getValue().getNextStepValue(1, -1, {});
  }
  else if(getVoiceGroup() == VoiceGroup::II)
  {
    return value < getValue().getNextStepValue(0, 1, {});
  }

  return false;
}

bool SplitPointParameter::hasOverlap()
{
  if(getVoiceGroup() == VoiceGroup::I)
  {
    return getControlPositionValue() > getSibling()->getControlPositionValue();
  }
  else
  {
    return getSibling()->hasOverlap();
  }
}

bool SplitPointParameter::inDefaultSplitBehaviour() const
{
  auto sib = getSibling();
  if(getVoiceGroup() == VoiceGroup::I)
  {
    auto sibPrev = sib->getNextStepValue(-1, {});
    return getValue().getQuantizedClipped() == sibPrev;
  }
  else
  {
    return sib->inDefaultSplitBehaviour();
  }
}
