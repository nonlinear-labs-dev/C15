#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/SplitPointSyncParameters.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameter_declarations.h>
#include <http/UndoScope.h>
#include <parameters/SplitPointParameter.h>
#include "SyncSplitSettingUseCases.h"

SyncSplitSettingUseCases& SyncSplitSettingUseCases::get()
{
  static SyncSplitSettingUseCases s;
  return s;
}

void SyncSplitSettingUseCases::enableSyncSetting(UNDO::Transaction* t)
{
  undoableSetSetting(t, true);

  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto sI = eb->findAndCastParameterByID<SplitPointParameter>({ C15::PID::Split_Split_Point, VoiceGroup::I });
  auto sII = eb->findAndCastParameterByID<SplitPointParameter>({ C15::PID::Split_Split_Point, VoiceGroup::II });

  auto cpI = sI->getControlPositionValue();
  auto cpII = sII->getValue().getNextStepValue(cpI, 1, {});
  sII->updateCPFromSyncChange(t, cpII);
}

void SyncSplitSettingUseCases::disableSyncSetting(UNDO::Transaction* t)
{
  undoableSetSetting(t, false);
}

SplitPointSyncParameters* SyncSplitSettingUseCases::getSyncSetting()
{
  return Application::get().getSettings()->getSetting<SplitPointSyncParameters>().get();
}

void SyncSplitSettingUseCases::undoableSetSetting(UNDO::Transaction* transaction, bool state)
{
  const auto oldState = getSyncSetting()->get();
  getSyncSetting()->setState(state);

  transaction->addSimpleCommand(
      [this, state](UNDO::Transaction::State s) {
        if(s == UNDO::Transaction::State::REDOING)
          getSyncSetting()->setState(state);
      },
      [this, oldState](auto s) { getSyncSetting()->setState(oldState); });
}

void SyncSplitSettingUseCases::enableSyncSetting()
{
  auto scope = Application::get().getPresetManager()->getUndoScope().startTransaction("Enable Split Sync");
  enableSyncSetting(scope->getTransaction());
}

void SyncSplitSettingUseCases::disableSyncSetting()
{
  auto scope = Application::get().getPresetManager()->getUndoScope().startTransaction("Disable Split Sync");
  disableSyncSetting(scope->getTransaction());
}

void SyncSplitSettingUseCases::updateFromWebUI(const Glib::ustring& value)
{
  if(value == "on")
  {
    enableSyncSetting();
  }
  else
  {
    disableSyncSetting();
  }
}
