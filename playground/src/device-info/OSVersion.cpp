#include <device-info/OSVersion.h>
#include <tools/SpawnCommandLine.h>
#include <xml/Writer.h>

OSVersion::OSVersion(DeviceInformation *parent)
    : DeviceInformationItem(parent)
{
  SpawnCommandLine cmd("uname -r");
  m_versionString = cmd.getStdOutputOrFallback("N/A");
}

void OSVersion::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  writer.writeTextElement("os-version", m_versionString);
}

Glib::ustring OSVersion::get() const
{
  return m_versionString;
}
