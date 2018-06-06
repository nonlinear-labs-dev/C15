#include "LayoutFactory.h"
#include "PrimitivesFactory.h"
#include "GenericLayout.h"
#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <tools/ExceptionTools.h>
#include "ControlClass.h"
#include "ControlInstance.h"
#include "Selector.h"
#include "TemplateEnums.h"
#include "EventSink.h"
#include "Styles.h"
#include "LayoutFolderMonitor.h"

namespace DescriptiveLayouts
{

  BoledLayoutFactory::BoledLayoutFactory()
  {
  }

  BoledLayoutFactory& BoledLayoutFactory::get()
  {
    static BoledLayoutFactory factory;
    return factory;
  }

  void BoledLayoutFactory::clear()
  {
    m_layouts.clear();
  }

  void BoledLayoutFactory::sortByPriority()
  {
    m_layouts.sort([](const LayoutClass & a, const LayoutClass & b) { return a.getWeight() > b.getWeight(); });
  }

  const DescriptiveLayouts::LayoutClass& BoledLayoutFactory::find(FocusAndMode fam) const
  {
    auto it = std::find_if(m_layouts.begin(), m_layouts.end(), [=](const LayoutClass& e)
    {
      return e.matches(fam) && e.meetsConditions();
    });

    if(it == m_layouts.end())
    {
      DebugLevel::throwException("No matching layout found! current modes:" ,
                                 toString(fam.focus),
                                 toString(fam.mode),
                                 toString(fam.detail));

    }

    DebugLevel::warning("Current Modes:",
                        toString(fam.focus),
                        toString(fam.mode),
                        toString(fam.detail));

    return *it;
  }

  std::shared_ptr<DFBLayout> BoledLayoutFactory::instantiate(FocusAndMode fam)
  {
    return std::shared_ptr<DFBLayout>(find(fam).instantiate());
  }

}
