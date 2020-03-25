#include "TextEditUsageMode.h"
#include "HWUI.h"
#include "ButtonRepeat.h"
#include <device-settings/DebugLevel.h>
#include <proxies/hwui/buttons.h>

#include <memory>

const gunichar c_shift = 0x12;
const gunichar c_symbol = 0x13;
const gunichar c_del = 0x7F;
const gunichar c_back = 0x8;
const gunichar c_space = 0x20;
const gunichar c_ins = 0x11;

static Glib::ustring layout[]
    = { u8"qwertyuiop[]", u8"asdfghjkl;'ü", u8"zxcvbnm,./öä", u8"\x12\x12\x7F\x7F\x8\x8\x20\x20\x11\x11\x13\x13" };

static Glib::ustring shiftLayout[]
    = { u8"QWERTYUIOP{}", u8"ASDFGHJKL:\"Ü", u8"ZXCVBNM<>?ÖÄ", u8"\x12\x12\x7F\x7F\x8\x8\x20\x20\x11\x11\x13\x13" };

static Glib::ustring symbolLayout[]
    = { u8"1234567890()", u8"~`!@#$%^&*_+", u8"|\\§°€    ß-=", u8"\x12\x12\x7F\x7F\x8\x8\x20\x20\x11\x11\x13\x13" };

static Glib::ustring symbolShiftLayout[]
    = { u8"1234567890()", u8"~`!@#$%^&*_+", u8"|\\§°€    ß-=", u8"\x12\x12\x7F\x7F\x8\x8\x20\x20\x11\x11\x13\x13" };

TextEditUsageMode::TextEditUsageMode(const Glib::ustring &initalText)
    : m_text(initalText)
    , m_position(initalText.length())
    , m_layout(Layout::Normal)
{
}

TextEditUsageMode::~TextEditUsageMode()
{
}

void TextEditUsageMode::setup()
{
  Application::get().getHWUI()->getPanelUnit().turnLedsOff();

  for(int col = 0; col < 12; col++)
  {
    for(int row = 0; row < 4; row++)
    {
      Buttons buttonID = (Buttons)(col * 4 + row);
      m_buttonMap[buttonID] = tButtonAssignment(layout[row].at(col), shiftLayout[row].at(col),
                                                symbolLayout[row].at(col), symbolShiftLayout[row].at(col));
      m_buttonMap[(Buttons)((int) buttonID + 48)] = tButtonAssignment(
          layout[row].at(col), shiftLayout[row].at(col), symbolLayout[row].at(col), symbolShiftLayout[row].at(col));
    }
  }
}

Glib::ustring TextEditUsageMode::getKeyLabel(Buttons buttonID) const
{
  try
  {
    auto uni = getCharForButton(buttonID);

    if(g_unichar_isgraph(uni))
    {
      Glib::ustring ret;
      ret = uni;
      return ret;
    }

    return stringizeSpecialChar(uni);
  }
  catch(std::out_of_range ex)
  {
    DebugLevel::error("there is no label for button", buttonID);
  }
  return "";
}

gunichar TextEditUsageMode::getCharForButton(Buttons buttonID) const
{
  auto keyAssignment = m_buttonMap.at(buttonID);

  switch(m_layout)
  {
    case Layout::Shift:
      return std::get<1>(keyAssignment);

    case Layout::Symbol:
      return std::get<2>(keyAssignment);

    case Layout::SymbolShift:
      return std::get<3>(keyAssignment);

    case Layout::Normal:
    default:
      return std::get<0>(keyAssignment);
      break;
  }
}

Glib::ustring TextEditUsageMode::stringizeSpecialChar(gunichar c) const
{
  switch(c)
  {
    case c_shift:
      return u8"Shift";

    case c_symbol:
      return u8"Symbol";

    case c_del:
      return u8"Del";

    case c_back:
      return u8"Back";

    case c_space:
      return u8" ";

    case c_ins:
      return u8"Ins";
  }

  return u8"Huh?";
}

void TextEditUsageMode::replaceCharWith(gunichar newChar)
{
  clampPosition();

  if(m_position >= 0 && m_position < m_text.length())
  {
    m_text.replace(m_position, 1, 1, newChar);
  }
  else
  {
    m_text += newChar;
  }

  m_position++;
  clampPosition();

  m_sigTextChanged.send(m_text);
}

void TextEditUsageMode::insertChar(gunichar newChar)
{
  clampPosition();

  if(m_position >= 0 && m_position < m_text.length())
  {
    m_text.insert(m_position, 1, newChar);
  }
  else
  {
    m_text += newChar;
  }

  m_position++;
  m_sigTextChanged.send(m_text);
}

void TextEditUsageMode::clampPosition()
{
  m_position = std::max<int>(m_position, 0);
  m_position = std::min<int>(m_position, m_text.length());
}

bool TextEditUsageMode::onButtonPressed(Buttons buttonID, ButtonModifiers modifiers, bool state)
{
  m_buttonRepeat.reset();

  if(buttonID == Buttons::BUTTON_SHIFT)
  {
    handleShiftButton(state);
    return true;
  }

  try
  {
    auto uni = getCharForButton(buttonID);

    if(g_unichar_isgraph(uni))
    {
      if(state)
      {
        installButtonRepeat([=]() { onCharPressed(uni); });
      }

      return true;
    }

    if(state)
    {
      return handleSpecialChar(uni);
    }
  }
  catch(std::out_of_range ex)
  {
  }

  return super::onButtonPressed(buttonID, modifiers, state);
}

void TextEditUsageMode::handleShiftButton(bool state)
{
  m_shiftState = state;
  chooseLayout();
}

void TextEditUsageMode::installButtonRepeat(std::function<void()> cb)
{
  m_buttonRepeat = std::make_unique<ButtonRepeat>(cb);
}

void TextEditUsageMode::onCharPressed(gunichar c)
{
  if(m_insert)
  {
    insertChar(c);
  }
  else
  {
    replaceCharWith(c);
  }
}

bool TextEditUsageMode::handleSpecialChar(gunichar c)
{
  switch(c)
  {
    case c_shift:
      toggleCapsLock();
      return true;

    case c_symbol:
      toggleSymbol();
      return true;

    case c_del:
      installButtonRepeat([=]() {
        if(m_position < m_text.length() && !m_text.empty())
        {
          m_text.erase(m_position, 1);
          m_sigTextChanged.send(m_text);
        }
      });

      return true;

    case c_back:
      installButtonRepeat([=]() {
        if(m_position > 0 && !m_text.empty())
        {
          m_position--;
          m_text.erase(m_position, 1);
          m_sigTextChanged.send(m_text);
        }
      });

      return true;

    case c_space:
      installButtonRepeat([=]() { onCharPressed(' '); });
      return true;

    case c_ins:
      m_insert = !m_insert;
      m_sigTextChanged.send(m_text);
      return true;
  }

  return false;
}

sigc::connection TextEditUsageMode::onTextChanged(const sigc::slot<void, const Glib::ustring &> &cb)
{
  return m_sigTextChanged.connectAndInit(cb, m_text);
}

gunichar TextEditUsageMode::getCharAtRelativeToCursorPosition(int pos) const
{
  pos += m_position;

  if(pos < 0)
    return c_space;

  if(pos >= m_text.length())
    return c_space;

  return m_text.at(pos);
}

bool TextEditUsageMode::isInserting() const
{
  return m_insert;
}

void TextEditUsageMode::moveCursor(int step)
{
  m_position += step;
  clampPosition();
  m_sigTextChanged.send(m_text);
}

Glib::ustring TextEditUsageMode::getText() const
{
  return m_text;
}

TextEditUsageMode::Layout TextEditUsageMode::getLayout() const
{
  return m_layout;
}

void TextEditUsageMode::chooseLayout()
{
  bool realShiftState = m_capsLock ^ m_shiftState;

  if(m_symbolState && !realShiftState)
    m_layout = Layout::Symbol;
  else if(m_symbolState && realShiftState)
    m_layout = Layout::SymbolShift;
  else if(!m_symbolState && !realShiftState)
    m_layout = Layout::Normal;
  else if(!m_symbolState && realShiftState)
    m_layout = Layout::Shift;

  updateLeds();
  m_sigTextChanged.send(m_text);
}

void TextEditUsageMode::toggleCapsLock()
{
  m_capsLock = !m_capsLock;
  chooseLayout();
}

void TextEditUsageMode::toggleSymbol()
{
  m_symbolState = !m_symbolState;
  chooseLayout();
}

void TextEditUsageMode::updateLeds()
{
  bool realShiftState = m_capsLock ^ m_shiftState;
  auto symbolState = m_layout == Layout::Symbol || m_layout == Layout::SymbolShift ? TwoStateLED::ON : TwoStateLED::OFF;
  auto capsState = realShiftState ? TwoStateLED::ON : TwoStateLED::OFF;
  auto hwui = Application::get().getHWUI();

  for(auto led : { Buttons::BUTTON_3, Buttons::BUTTON_7, Buttons::BUTTON_51, Buttons::BUTTON_55 })
  {
    hwui->getPanelUnit().getLED(led)->setState(capsState);
  }

  for(auto led : { Buttons::BUTTON_43, Buttons::BUTTON_47, Buttons::BUTTON_91, Buttons::BUTTON_95 })
  {
    hwui->getPanelUnit().getLED(led)->setState(symbolState);
  }
}