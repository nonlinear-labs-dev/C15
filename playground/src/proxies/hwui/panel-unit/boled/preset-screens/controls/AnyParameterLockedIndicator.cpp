#include "AnyParameterLockedIndicator.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/Parameter.h"
#include "proxies/hwui/panel-unit/boled/BOLED.h"

AnyParameterLockedIndicator::AnyParameterLockedIndicator(const Rect &pos) :
    super(pos),
    m_calcHasLocks(1, std::bind(&AnyParameterLockedIndicator::calcHasLocks, this))
{
  setFontColor (FrameBuffer::C43);
  Application::get().getPresetManager()->getEditBuffer()->onLocksChanged(sigc::mem_fun(this, &AnyParameterLockedIndicator::update));
  update();
}

AnyParameterLockedIndicator::~AnyParameterLockedIndicator()
{
}

void AnyParameterLockedIndicator::calcHasLocks()
{
  auto hasLocks = Application::get().getPresetManager()->getEditBuffer()->hasLocks();
  setText(hasLocks ? "L" : "");
  setVisible(hasLocks);
}

void AnyParameterLockedIndicator::update()
{
  m_calcHasLocks.trigger();
}


