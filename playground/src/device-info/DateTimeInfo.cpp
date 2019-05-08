#include <Application.h>
#include <device-info/DateTimeInfo.h>
#include <device-settings/DateTimeAdjustment.h>
#include <device-settings/Settings.h>
#include <xml/Writer.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <tools/TimeTools.h>

DateTimeInfo::DateTimeInfo(DeviceInformation *parent)
    : DeviceInformationItem(parent)
{
  onTimeChanged();
  Application::get().getMainContext()->signal_timeout().connect_seconds(
      sigc::mem_fun(this, &DateTimeInfo::onTimeChanged), 60);
}

bool DateTimeInfo::onTimeChanged()
{
  onChange(UpdateDocumentContributor::ChangeFlags::Generic);
  return true;
}

void DateTimeInfo::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  writer.writeTextElement("date-time", get());
  writer.writeTextElement("date-time-display", getDisplayString());
}

Glib::ustring DateTimeInfo::get() const
{
  return TimeTools::getAdjustedIso();
}

Glib::ustring DateTimeInfo::getDisplayString() const
{
  return TimeTools::getDisplayStringFromIso(TimeTools::getAdjustedIso());
}
