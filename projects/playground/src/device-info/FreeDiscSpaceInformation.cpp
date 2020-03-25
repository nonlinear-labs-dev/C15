#include <device-info/FreeDiscSpaceInformation.h>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
#include <sigc++/functors/mem_fun.h>
#include <nltools/system/SpawnCommandLine.h>
#include <xml/Writer.h>
#include <chrono>

FreeDiscSpaceInformation::FreeDiscSpaceInformation(DeviceInformation* parent)
    : DeviceInformationItem(parent)
    , m_value("N/A")
{
  refresh();

  using namespace std::chrono;
  constexpr minutes timeout(5);
  Glib::MainContext::get_default()->signal_timeout().connect(sigc::mem_fun(this, &FreeDiscSpaceInformation::refresh),
                                                             duration_cast<milliseconds>(timeout).count());
}

bool FreeDiscSpaceInformation::refresh()
{
  SpawnCommandLine cmd("sh -c \"df -h | grep '/persistent' | awk '{print $4}'\"");

  std::string newValue = cmd.getStdOutputOrFallback("N/A");

  std::string trimmed {};
  std::copy_if(newValue.begin(), newValue.end(), std::back_inserter(trimmed),
               [](const char o) { return std::isalnum(o); });

  if(m_value != trimmed)
  {
    m_value = trimmed;
    onChange();
  }

  return true;
}

void FreeDiscSpaceInformation::writeDocument(Writer& writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  writer.writeTextElement("free-disc-space", m_value);
}

Glib::ustring FreeDiscSpaceInformation::get() const
{
  return m_value;
}
