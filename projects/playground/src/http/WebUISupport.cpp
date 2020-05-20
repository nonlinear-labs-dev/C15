#include "WebUISupport.h"
#include <device-settings/PedalType.h>
#include <xml/OutStream.h>
#include <http/NetworkRequest.h>
#include <http/HTTPRequest.h>

#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/PedalType.h>

namespace
{
  std::string getPedalStrings()
  {
    std::stringstream str;

    auto types = Application::get().getSettings()->getSetting<PedalType>();

    for(const auto name : types->enumToDisplayString())
    {
      if(str.tellp())
        str << '\n';
      str << name;
    }

    return str.str();
  }
}

WebUISupport::WebUISupport(UpdateDocumentContributor *master)
    : ContentSection(master)
    , RPCActionManager("/webui-support/")
{
  addAction("enum/get-strings", [&](std::shared_ptr<NetworkRequest> request) {
    if(auto h = std::dynamic_pointer_cast<HTTPRequest>(request))
    {
      h->setStatusOK();

      if(h->get("name") == "PedalTypes")
        h->respond(getPedalStrings());

      h->okAndComplete();
    }
  });
}

void WebUISupport::handleHTTPRequest(std::shared_ptr<NetworkRequest> request, const Glib::ustring &)
{
  RPCActionManager::handleRequest(request);
}

Glib::ustring WebUISupport::getPrefix() const
{
  return getBasePath().substr(1);
}

void WebUISupport::writeDocument(Writer &, UpdateDocumentContributor::tUpdateID) const
{
}
