#include <Application.h>
#include <groups/MacroControlsGroup.h>
#include <tools/StringTools.h>
#include <parameters/MacroControlParameter.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ModulationBoundLabel.h>
#include <proxies/hwui/FrameBuffer.h>

ModulationBoundLabel::ModulationBoundLabel(const Rect &r)
    : super(r)
{
  Application::get().getPresetManager()->getEditBuffer()->onSelectionChanged(
      mem_fun(this, &ModulationBoundLabel::onParameterSelectionChanged), getHWUI()->getCurrentVoiceGroup());

  Application::get().getHWUI()->onModifiersChanged(
      sigc::mem_fun(this, &ModulationBoundLabel::onButtonModifiersChanged));
}

ModulationBoundLabel::~ModulationBoundLabel()
{
}

void ModulationBoundLabel::onParameterSelectionChanged(Parameter *oldParam, Parameter *newParam)
{
  m_paramConnection.disconnect();

  if((m_modulatedParam = dynamic_cast<ModulateableParameter *>(newParam)))
  {
    m_paramConnection
        = m_modulatedParam->onParameterChanged(sigc::mem_fun(this, &ModulationBoundLabel::onParameterChanged));
  }
  else
  {
    setText(StringAndSuffix::empty());
  }
}

void ModulationBoundLabel::onParameterChanged(const Parameter *param)
{
  if(auto m = dynamic_cast<const ModulateableParameter *>(param))
  {
    if(m_mc != m->getModulationSource())
    {
      m_mc = m->getModulationSource();
      m_mcConnection.disconnect();

      auto mcID = MacroControlsGroup::modSrcToParamId(m_mc);
      auto editBuffer = Application::get().getPresetManager()->getEditBuffer();

      if((m_mcParam = dynamic_cast<MacroControlParameter *>(editBuffer->findParameterByID(mcID))))
      {
        m_mcConnection
            = m_mcParam->onParameterChanged(sigc::mem_fun(this, &ModulationBoundLabel::onMCParameterChanged));
      }
      else
      {
        setText(StringAndSuffix::empty());
      }
    }
  }

  updateText();
}

void ModulationBoundLabel::onMCParameterChanged(const Parameter *mcParam)
{
  updateText();
}

void ModulationBoundLabel::onButtonModifiersChanged(ButtonModifiers mod)
{
  updateText();
}

void ModulationBoundLabel::updateText()
{
  if(m_mcParam && m_modulatedParam)
    updateText(m_mcParam, m_modulatedParam);
}

ModulateableParameter *ModulationBoundLabel::getModulatedParameter()
{
  return m_modulatedParam;
}

void ModulationBoundLabel::setSuffixFontColor(FrameBuffer &fb) const
{
  fb.setColor(FrameBufferColors::C103);
}

StringAndSuffix ModulationBoundLabel::shortenStringIfNeccessary(const std::shared_ptr<Font> &font,
                                                                const StringAndSuffix &text) const
{
  if(font->getStringWidth(text.text) >= getWidth())
  {
    auto sText = StringTools::removeSpaces(text.text);
    if(sText.find_last_of('F') != Glib::ustring::npos)
      return StringAndSuffix { sText, 1 };
    return StringAndSuffix { sText };
  }
  return StringAndSuffix { text };
}
