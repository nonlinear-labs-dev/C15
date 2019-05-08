#include <Application.h>
#include <presets/EditBuffer.h>
#include <presets/PresetBank.h>
#include <presets/PresetManager.h>
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

  auto eb = Application::get().getPresetManager()->getEditBuffer();
  eb->onPresetLoaded(mem_fun(this, &PresetListEntry::onPresetChanged));
}

PresetListEntry::~PresetListEntry()
{
  doAnimationCallback();
  m_animationConnection.disconnect();
  m_presetConnection.disconnect();
}

void PresetListEntry::setPreset(std::shared_ptr<Preset> preset, bool selected)
{
  m_presetConnection.disconnect();

  m_preset = preset;
  m_selected = selected;

  if(m_preset)
  {
    m_presetConnection = m_preset->onPresetChanged(mem_fun(this, &PresetListEntry::onPresetChanged));
  }
  else
  {
    onPresetChanged();
  }
}

void PresetListEntry::onPresetChanged()
{
  if(m_preset)
  {
    bool isLoaded
        = m_preset->getUuid() == Application::get().getPresetManager()->getEditBuffer()->getUUIDOfLastLoadedPreset();
    auto bank = m_preset->getBank();
    auto num = bank->getPresetPosition(m_preset->getUuid());
    m_number->update(num, m_selected, isLoaded);
    m_name->update(m_preset->getName(), m_selected, isLoaded);
  }
  else
  {
    m_number->update(-1, false, false);
    m_name->update("", false, false);
  }
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
