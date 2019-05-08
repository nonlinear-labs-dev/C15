#include <device-info/FreeDiscSpaceInformation.h>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
#include <sigc++/connection.h>
#include <sigc++/functors/mem_fun.h>
#include <tools/SpawnCommandLine.h>
#include <xml/Writer.h>
#include <chrono>

FreeDiscSpaceInformation::FreeDiscSpaceInformation(DeviceInformation *parent)
    : DeviceInformationItem(parent)
    , m_value("N/A")
{
  refresh();

  using namespace std::chrono;
  constexpr minutes timeout(5);
  Glib::MainContext::get_default()->signal_timeout().connect(mem_fun(this, &FreeDiscSpaceInformation::refresh),
                                                             duration_cast<milliseconds>(timeout).count());
}

bool FreeDiscSpaceInformation::refresh()
{
  SpawnCommandLine cmd(
      "sh -c \"df -h | grep '/dev/root' | sed 's/^\\S*\\s*\\S*\\s*\\S*\\s*\\(\\S*\\)\\s*.*/\\1/' | tr '\\n'  ' ' \" ");

  Glib::ustring newValue = cmd.getStdOutputOrFallback("N/A");

  if(m_value != newValue)
  {
    m_value = newValue;
    onChange();
  }

  return true;
}

void FreeDiscSpaceInformation::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  writer.writeTextElement("free-disc-space", m_value);
}

Glib::ustring FreeDiscSpaceInformation::get() const
{
  return m_value;
}
