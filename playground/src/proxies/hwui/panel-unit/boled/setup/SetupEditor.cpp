#include "SetupEditor.h"
#include "SetupLayout.h"

SetupEditor::SetupEditor()
{
}

SetupEditor::~SetupEditor()
{
}

bool SetupEditor::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  return false;
}

bool SetupEditor::onRotary(int inc, ButtonModifiers modifiers)
{
  return false;
}

void SetupEditor::setSetupLayout(SetupLayout* layout)
{
  m_setupLayout = layout;
}

void SetupEditor::diveUp()
{
  m_setupLayout->diveUp();
}
