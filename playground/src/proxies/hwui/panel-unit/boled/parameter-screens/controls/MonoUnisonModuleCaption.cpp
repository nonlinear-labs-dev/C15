#include "MonoUnisonModuleCaption.h"
#include <proxies/hwui/HWUI.h>
#include <groups/MonoGroup.h>

void MonoUnisonModuleCaption::updateText(Parameter *newOne)
{
  if(auto group = newOne->getParentGroup())
  {
    auto groupStr = group->getShortName();

    if(auto editBuffer = dynamic_cast<EditBuffer *>(group->getParent()))
    {
      if(editBuffer->getType() == SoundType::Layer)
      {
        if(UnisonGroup::isUnisonParameter(newOne) || MonoGroup::isMonoParameter(newOne))
        {
          setText({ groupStr + " I/II", 0 });
          return;
        }
      }
      else if(editBuffer->getType() == SoundType::Single)
      {
        if(UnisonGroup::isUnisonParameter(newOne) || MonoGroup::isMonoParameter(newOne))
        {
          setText({ groupStr, 0 });
          return;
        }
      }
    }

    setText({ groupStr + " " + toString(Application::get().getHWUI()->getCurrentVoiceGroup()), 0 });
    return;
  }

  ModuleCaption::updateText(newOne);
}
