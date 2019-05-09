#include "LayoutConditionDatabase.h"

#include "Application.h"
#include "device-settings/DebugLevel.h"
#include "parameters/ModulateableParameter.h"
#include "presets/EditBuffer.h"
#include "presets/PresetManager.h"
#include <unordered_map>
#include <functional>

namespace DescriptiveLayouts
{
  static struct LayoutConditionDatabase
  {
    LayoutConditionDatabase()
    {
      m_conditions[LayoutConditions::IsParameterModulateable] = []() {
        auto p = Application::get().getPresetManager()->getEditBuffer()->getSelected();
        return dynamic_cast<ModulateableParameter*>(p) != nullptr;
      };
    }
    using Condition = std::function<bool()>;
    std::unordered_map<LayoutConditions, Condition> m_conditions;
  } db;

  bool isConditionFulfilled(LayoutConditions c)
  {
    try
    {
      return db.m_conditions.at(c)();
    }
    catch(...)
    {
      DebugLevel::throwException("Condition", toString(c), "is not implemented.");
    }
    return false;
  }
}
