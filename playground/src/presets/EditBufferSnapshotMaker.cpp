#include "EditBufferSnapshotMaker.h"
#include <presets/PresetManager.h>
#include "EditBuffer.h"
#include <Application.h>
#include <http/UndoScope.h>
#include <device-settings/DebugLevel.h>
#include <parameters/PhysicalControlParameter.h>
#include <parameters/ModulationRoutingParameter.h>
#include <parameters/MacroControlParameter.h>
#include <libundo/undo/Transaction.h>

EditBufferSnapshotMaker &EditBufferSnapshotMaker::get()
{
  static EditBufferSnapshotMaker theMaker;
  return theMaker;
}

EditBufferSnapshotMaker::EditBufferSnapshotMaker()
{
}

void EditBufferSnapshotMaker::addSnapshotIfRequired(UNDO::Transaction *transaction)
{
  auto editBuffer = Application::get().getPresetManager()->getEditBuffer();
  auto hardwareSources = editBuffer->getParameterGroupByID("CS");
  auto updateID = hardwareSources->getUpdateIDOfLastChange();

  if(updateID != m_lastKnownUpdateID)
  {
    tParams params = collectDirtyParameters(editBuffer);

    if(!params.empty())
    {
      addSnapshot(transaction, std::move(params));
    }

    m_lastKnownUpdateID = updateID;
  }
}

auto EditBufferSnapshotMaker::collectDirtyParameters(EditBuffer *editBuffer) -> tParams
{
  EditBufferSnapshotMaker::tParams ret;

  for(auto group : editBuffer->getParameterGroups())
  {
    if(group->getUpdateIDOfLastChange() > m_lastKnownUpdateID)
    {
      for(auto param : group->getParameters())
      {
        if(param->getUpdateIDOfLastChange() > m_lastKnownUpdateID)
        {
          auto snValue = param->expropriateSnapshotValue();
          auto curValue = param->getControlPositionValue();

          if(snValue != curValue)
          {
            ret.push_back({ param, param->expropriateSnapshotValue() });
          }
        }
      }
    }
  }

  return std::move(ret);
}

void EditBufferSnapshotMaker::addSnapshot(UNDO::Transaction *transaction, tParams &&params)
{
  DebugLevel::info("Some parameters changed since last snapshot -> so creating a new one");

  sortParametersByModulationFlow(params);

  auto swapData = UNDO::createSwapData(std::move(params));

  transaction->addSimpleCommand([=](UNDO::Command::State state) mutable {
    if(state != UNDO::Command::State::DOING)
    {
      tParams &params = swapData->get<0>();

      for(auto &record : params)
      {
        auto currentValue = record.parameter->getControlPositionValue();
        std::swap(currentValue, record.snapshotValue);
        record.parameter->getValue().setRawValue(Initiator::EXPLICIT_OTHER, currentValue);
        record.parameter->sendToLpc();
      }
    }
  });
}

void EditBufferSnapshotMaker::sortParametersByModulationFlow(tParams &params)
{
  auto firstNonPhysical = std::partition(params.begin(), params.end(), [](const Record &r) {
    return dynamic_cast<PhysicalControlParameter *>(r.parameter);
  });

  auto firstNonRouter = std::partition(firstNonPhysical, params.end(), [](const Record &r) {
    return dynamic_cast<ModulationRoutingParameter *>(r.parameter);
  });

  std::partition(firstNonRouter, params.end(),
                 [](const Record &r) { return dynamic_cast<MacroControlParameter *>(r.parameter); });
}
