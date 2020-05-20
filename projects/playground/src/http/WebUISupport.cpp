#include "WebUISupport.h"
#include <device-settings/PedalType.h>
#include <xml/OutStream.h>
#include <http/NetworkRequest.h>
#include <http/HTTPRequest.h>

namespace
{
  template <typename T> std::string getEnumStrings()
  {
    std::stringstream str;

#warning "TODO: not return enum-string, but preset name base on enum"
    for(const auto name : getAllStrings<T>())
    {
      if(str.tellp())
        str << ',';
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
        h->respond(getEnumStrings<PedalTypes>());

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
