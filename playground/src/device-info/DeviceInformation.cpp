#include "DeviceInformation.h"
#include "FreeDiscSpaceInformation.h"
#include "SoftwareVersion.h"
#include <xml/Attribute.h>
#include <xml/Writer.h>
#include "device-settings/DebugLevel.h"
#include "RTSoftwareVersion.h"
#include "OSVersion.h"
#include "DateTimeInfo.h"
#include <Application.h>
#include <proxies/lpc/LPCProxy.h>
#include <http/NetworkRequest.h>

DeviceInformation::DeviceInformation(UpdateDocumentContributor *parent)
    : ContentSection(parent)
    , m_actions("device-info")
{
  m_items.emplace_back(new FreeDiscSpaceInformation(this));
  m_items.emplace_back(new SoftwareVersion(this));
  m_items.emplace_back(new RTSoftwareVersion(this));
  m_items.emplace_back(new OSVersion(this));
  m_items.emplace_back(new DateTimeInfo(this));

  m_actions.addAction("refresh-rt-software-version", [](shared_ptr<NetworkRequest> request) {
    Application::get().getLPCProxy()->requestLPCSoftwareVersion();
    request->okAndComplete();
  });
}

DeviceInformation::~DeviceInformation()
{
  DebugLevel::warning(__PRETTY_FUNCTION__, __LINE__);
}

void DeviceInformation::handleHTTPRequest(shared_ptr<NetworkRequest> request, const Glib::ustring &path)
{
  ContentSection::handleHTTPRequest(request, path);

  if(m_actions.matches(path))
    if(m_actions.handleRequest(path, request))
      return;

  DebugLevel::warning("could not handle request", path);
}

Glib::ustring DeviceInformation::getPrefix() const
{
  return "device-info";
}

Glib::ustring DeviceInformation::getSoftwareVersion() const
{
  for(auto a : m_items)
    if(auto b = dynamic_pointer_cast<SoftwareVersion>(a))
      return b->get();

  return "";
}

void DeviceInformation::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  bool changed = knownRevision < getUpdateIDOfLastChange();

  writer.writeTag("device-information", Attribute("changed", changed), [&]() {
    if(changed)
    {
      for(auto &info : m_items)
      {
        info->writeDocument(writer, knownRevision);
      }
    }
  });
}
