#include "TestEditorItem.h"
#include "../Menu/GenericMenuEditor.h"

GenericMenuEditor* TestEditorItem::createEditor()
{
  struct Foo : public GenericMenuEditor
  {
    Foo()
    {
      addControl(new LeftAlignedLabel({ "Foo", 0 }, { 0, 0, 128, 15 }));
      m_label = addControl(new LeftAlignedLabel({ "Value", 0 }, { 0, 15, 128, 15 }));
    }

    bool onButton(Buttons button, bool down, ButtonModifiers modifiers) override
    {
      if(button != Buttons::BUTTON_A && button != Buttons::BUTTON_B) {
        m_label->setText({toString(button), 0});
        return true;
      } else {
        return false;
      }
    }

    LeftAlignedLabel* m_label;
  };

  return new Foo();
}

void TestEditorItem::doAction() {
  //nothing
}
