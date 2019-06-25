#include <Application.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/NumPresetsInBankLabel.h>
#include <presets/Bank.h>

NumPresetsInBankLabel::NumPresetsInBankLabel(const Rect &pos)
    : super(pos)
{
  Application::get().getPresetManager()->onBankSelection(mem_fun(this, &NumPresetsInBankLabel::updateText));
}

NumPresetsInBankLabel::~NumPresetsInBankLabel()
{
}

void NumPresetsInBankLabel::updateText(const Uuid &selectedBank)
{
  if(auto bank = Application::get().getPresetManager()->getSelectedBank())
  {
    auto str = UNDO::StringTools::buildString('[', bank->getNumPresets(), ']');
    setText(str);
  }
  else
  {
    setText("");
  }
}

void NumPresetsInBankLabel::setFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBuffer::Colors::C103);
}
