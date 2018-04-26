#include "RenameLetter.h"
#include <proxies/hwui/TextEditUsageMode.h>

RenameLetter::RenameLetter (shared_ptr<TextEditUsageMode> mode, Buttons buttonID, const Rect &pos) :
  super (pos),
  m_mode (mode),
  m_buttonID (buttonID)
{
}

Glib::ustring RenameLetter::getText() const
{
  return m_mode->getKeyLabel (m_buttonID);
}

bool RenameLetter::redraw (FrameBuffer &fb)
{
  Rect r = getPosition ();

  if(highlightLetter())
  {
    fb.setColor (FrameBuffer::Colors::C77);
    fb.fillRect(r.getLeft () + 1, r.getTop (), r.getWidth () - 2, r.getHeight () - 1);
  }

  super::redraw (fb);
  fb.drawRect (r.getLeft () + 1, r.getTop (), r.getWidth () - 2, r.getHeight () - 1);
  return true;
}

bool RenameLetter::highlightLetter() const
{
  if(m_mode->getLayout() == TextEditUsageMode::TextEditUsageMode::Layout::Shift && (m_buttonID == Buttons::BUTTON_3 || m_buttonID == Buttons::BUTTON_7))
    return true;

  if(m_mode->getLayout() == TextEditUsageMode::TextEditUsageMode::Layout::Symbol && (m_buttonID == Buttons::BUTTON_43 || m_buttonID == Buttons::BUTTON_47))
    return true;

  if (m_mode->getLayout() == TextEditUsageMode::TextEditUsageMode::Layout::SymbolShift
      && (m_buttonID == Buttons::BUTTON_3 || m_buttonID == Buttons::BUTTON_7 || m_buttonID == Buttons::BUTTON_43 || m_buttonID == Buttons::BUTTON_47))
    return true;

  return false;
}


int RenameLetter::getXOffset() const
{
  return 1;
}
