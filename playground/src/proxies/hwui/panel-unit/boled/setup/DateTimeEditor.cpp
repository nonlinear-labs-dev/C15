#include <Application.h>
#include <device-settings/DateTimeAdjustment.h>
#include <device-settings/Settings.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/controls/Button.h>
#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/setup/DateTimeEditor.h>
#include <cstdio>
#include <ctime>

DateTimeEditor::DateTimeEditor()
    : ControlWithChildren(Rect(0, 0, 0, 0))
    , m_originalTime(std::time(nullptr))
{
  auto margin = 5;
  auto y = 12;
  auto w = 20;

  m_labels[Selection::Year] = addControl(new Label("yyyy", Rect(margin, y, 2 * w, 10)));
  m_labels[Selection::Month] = addControl(new Label("mm", Rect(margin + 2 * w, y, w, 10)));
  m_labels[Selection::Day] = addControl(new Label("dd", Rect(margin + 3 * w, y, w, 10)));

  m_labels[Selection::Hour] = addControl(new Label("HH", Rect(margin + 4 * w, y, w, 10)));
  m_labels[Selection::Minute] = addControl(new Label("MM", Rect(margin + 5 * w, y, w, 10)));

  y = 24;

  m_controls[Selection::Year] = addControl(new Label("", Rect(margin, y, 2 * w, 10)));
  m_controls[Selection::Month] = addControl(new Label("", Rect(margin + 2 * w, y, w, 10)));
  m_controls[Selection::Day] = addControl(new Label("", Rect(margin + 3 * w, y, w, 10)));

  m_controls[Selection::Hour] = addControl(new Label("", Rect(margin + 4 * w, y, w, 10)));
  m_controls[Selection::Minute] = addControl(new Label("", Rect(margin + 5 * w, y, w, 10)));

  auto buttonHeight = 11;
  auto buttonWidth = 58;
  auto buttonMargin = 3;

  addControl(new Button(
      "<", Rect(buttonMargin, c_fullRightSidePosition.getHeight() - buttonHeight, buttonWidth, buttonHeight)));
  addControl(new Button(">",
                        Rect(buttonMargin + buttonWidth + 6, c_fullRightSidePosition.getHeight() - buttonHeight,
                             buttonWidth, buttonHeight)));

  m_labels[m_selection]->setHighlight(true);
  m_controls[m_selection]->setHighlight(true);

  m_diff = Application::get().getSettings()->getSetting<DateTimeAdjustment>()->get();

  setTimeValues();
}

DateTimeEditor::~DateTimeEditor() = default;

void DateTimeEditor::setPosition(const Rect &)
{
  ControlWithChildren::setPosition(c_fullRightSidePosition);
}

bool DateTimeEditor::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(down)
  {
    m_labels[m_selection]->setHighlight(false);
    m_controls[m_selection]->setHighlight(false);

    if(i == Buttons::BUTTON_C)
    {
      m_selection = step(m_selection, -1);
    }

    if(i == Buttons::BUTTON_D)
    {
      m_selection = step(m_selection, 1);
    }

    m_labels[m_selection]->setHighlight(true);
    m_controls[m_selection]->setHighlight(true);
    setAllDirty();
    return true;
  }
  return false;
}

bool DateTimeEditor::onRotary(int inc, ButtonModifiers modifiers)
{
  std::time_t t = m_originalTime + m_diff;
  std::tm *tm = std::localtime(&t);

  switch(m_selection)
  {
    case Month:
      tm->tm_mon += inc;
      break;

    case Day:
      tm->tm_mday += inc;
      break;

    case Year:
      tm->tm_year += inc;
      tm->tm_year = std::min(tm->tm_year, 2050 - 1900);
      tm->tm_year = std::max(tm->tm_year, 2017 - 1900);
      break;

    case Hour:
      tm->tm_hour += inc;
      break;

    case Minute:
      tm->tm_min += inc;
      break;

    default:
      break;
  }

  auto modifiedTime = timelocal(tm);
  Application::get().getSettings()->getSetting<DateTimeAdjustment>()->adjust(modifiedTime);
  m_diff = modifiedTime - m_originalTime;
  setTimeValues();
  return true;
}

Glib::ustring format(int v, int numDigits)
{
  char format[64];
  sprintf(format, "%%0%dd", numDigits);
  char txt[64];
  sprintf(txt, format, v);
  return txt;
}

void DateTimeEditor::setTimeValues()
{
  std::time_t t = std::time(nullptr) + m_diff;
  std::tm *tm = std::localtime(&t);

  m_controls[Selection::Month]->setText(format(tm->tm_mon + 1, 2));
  m_controls[Selection::Day]->setText(format(tm->tm_mday, 2));
  m_controls[Selection::Year]->setText(format(tm->tm_year + 1900, 4));
  m_controls[Selection::Hour]->setText(format(tm->tm_hour, 2));
  m_controls[Selection::Minute]->setText(format(tm->tm_min, 2));
  setAllDirty();
}

const DateTimeEditor::Selection DateTimeEditor::step(Selection s, int inc) const
{
  auto currentRaw = static_cast<int>(s);

  if(inc > 0)
    currentRaw += inc;
  else if(inc < 0)
    currentRaw += static_cast<int>(Selection::NumFields + inc);

  return static_cast<Selection>(currentRaw % Selection::NumFields);
}
