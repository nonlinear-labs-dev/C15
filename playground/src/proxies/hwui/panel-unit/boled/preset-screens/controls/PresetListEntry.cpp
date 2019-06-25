#include <Application.h>
#include <presets/EditBuffer.h>
#include <presets/Bank.h>
#include <presets/PresetManager.h>
#include <presets/Preset.h>
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListEntry.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetNameLabel.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetNumberLabel.h>
#include <functional>

const float c_numAnimationSteps = 30;
const float c_animationLength = 1000;  // ms

PresetListEntry::PresetListEntry(const Rect &pos)
    : super(pos)
    , m_animationProgress(0)
{
  m_number = addControl(new PresetNumberLabel(Rect(0, 0, 21, 16)));
  m_name = addControl(new PresetNameLabel(Rect(21, 0, 105, 16)));
}

PresetListEntry::~PresetListEntry()
{
  m_animationConnection.disconnect();
  m_presetConnection.disconnect();
}

void PresetListEntry::setPreset(Preset *preset, bool selected)
{
  m_presetConnection.disconnect();
  m_selected = selected;

  if(preset)
    m_presetConnection = preset->onChanged(sigc::bind<0>(mem_fun(this, &PresetListEntry::onPresetChanged), preset));
  else
    onPresetChanged(nullptr);
}

void PresetListEntry::onPresetChanged(const Preset *preset)
{
  if(preset)
  {
    bool isLoaded
        = preset->getUuid() == Application::get().getPresetManager()->getEditBuffer()->getUUIDOfLastLoadedPreset();
    if(auto bank = dynamic_cast<Bank *>(preset->getParent()))
    {
      auto num = bank->getPresetPosition(preset->getUuid());
      m_number->update(num, m_selected, isLoaded);
      m_name->update(preset->getName(), m_selected, isLoaded);
      return;
    }
  }

  m_number->update(-1, false, false);
  m_name->update("", false, false);
}

bool PresetListEntry::redraw(FrameBuffer &fb)
{
  super::redraw(fb);

  if(m_selected)
  {
    if(isHighlight())
      fb.setColor(FrameBuffer::Colors::C179);
    else
      fb.setColor(FrameBuffer::Colors::C103);

    const Rect &r = getPosition();
    fb.drawRect(r.getLeft() + 1, r.getTop(), r.getWidth() - 2, r.getHeight());
  }

  if(m_animationProgress)
  {
    Rect r = getPosition();

    if(m_animationProgress <= 50)
      r.setWidth(r.getWidth() * 2 * m_animationProgress / 100.0f);
    else
      r.setLeft(r.getLeft() + r.getWidth() * 2 * (m_animationProgress - 50) / 100.0f);

    fb.setColor(FrameBuffer::Colors::C255);
    fb.fillRect(r.getLeft(), r.getTop(), r.getWidth(), r.getHeight());
  }

  return true;
}

void PresetListEntry::animate(tCallback cb)
{
  m_animationConnection.disconnect();

  m_animationCallback = cb;
  m_animationProgress = 0;
  int interval = c_animationLength / c_numAnimationSteps;
  auto animation = sigc::mem_fun(this, &PresetListEntry::animationProgress);
  m_animationConnection = MainContext::get_default()->signal_timeout().connect(animation, interval);
}

bool PresetListEntry::animationProgress()
{
  setDirty();

  m_animationProgress += 100.0f / c_numAnimationSteps;

  if(m_animationProgress > 100)
  {
    m_animationProgress = 0;
    doAnimationCallback();
  }

  return m_animationProgress != 0;
}

void PresetListEntry::doAnimationCallback()
{
  tCallback cb;
  swap(cb, m_animationCallback);

  if(cb)
    cb();
}
