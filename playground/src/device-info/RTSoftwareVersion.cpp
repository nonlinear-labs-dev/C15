#include "RTSoftwareVersion.h"
#include <xml/Writer.h>
#include <xml/Attribute.h>
#include <device-settings/DebugLevel.h>
#include <Application.h>
#include <proxies/lpc/LPCProxy.h>

RTSoftwareVersion::RTSoftwareVersion(DeviceInformation *parent)
    : DeviceInformationItem(parent)
{
  m_connection = Application::get().getLPCProxy()->onLPCSoftwareVersionChanged(
      sigc::mem_fun(this, &RTSoftwareVersion::onVersionChanged));
}

void RTSoftwareVersion::onVersionChanged(int v)
{
  onChange();
}

void RTSoftwareVersion::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  writer.writeTextElement("rt-software-version", get());
}

Glib::ustring RTSoftwareVersion::get() const
{
  return to_string(Application::get().getLPCProxy()->getLPCSoftwareVersion());
}
