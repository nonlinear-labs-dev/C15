#include "WebUISupport.h"
#include <device-settings/PedalType.h>
#include <xml/OutStream.h>
#include <http/NetworkRequest.h>
#include <http/HTTPRequest.h>
#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/PedalType.h>
#include <proxies/hwui/HWUI.h>

namespace
{
  template <typename T> std::string getEnumStrings()
  {
    std::string result;
    auto names = Application::get().getSettings()->getSetting<PedalType>()->getDisplayStrings();

    for(const auto name : names)
      result.append(result.empty() ? 0 : 1, '\n').append(name);

    return result;
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

void WebUISupport::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID) const
{
  writer.writeTag("webui-helper", [&]() {
    auto selectedVG = Application::get().getHWUI()->getCurrentVoiceGroup();
    writer.writeTextElement("selected-part", toString(selectedVG));
  });
}
