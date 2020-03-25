#include <device-info/OSVersion.h>
#include <nltools/system/SpawnCommandLine.h>
#include <xml/Writer.h>

OSVersion::OSVersion(DeviceInformation *parent)
    : DeviceInformationItem(parent)
{
  SpawnCommandLine cmd("uname -r");
  m_versionString = cmd.getStdOutputOrFallback("N/A");
  auto it = std::find(m_versionString.begin(), m_versionString.end(), '\n');
  if(it != m_versionString.end())
  {
    m_versionString.erase(it, m_versionString.end());
  }
}

void OSVersion::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  writer.writeTextElement("os-version", m_versionString);
}

Glib::ustring OSVersion::get() const
{
  return m_versionString;
}
