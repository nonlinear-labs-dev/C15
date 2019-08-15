#include <proxies/hwui/descriptive-layouts/Concrete/Menu/ActionItem.h>
#include <nltools/logging/Log.h>
#include "GenericEditMenu.h"
#include "TestEditorItem.h"

GenericEditMenu::GenericEditMenu()
    : ScrollMenu()
{
  init();
}

void GenericEditMenu::init()
{
  addItem<ActionItem>("Foo", [] { nltools::Log::warning("Foo Action Pressed!"); });

  addItem<TestEditorItem>();
}
