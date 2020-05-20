#pragma once

#include "playground.h"
#include "http/ContentSection.h"
#include "http/RPCActionManager.h"

class WebUISupport : public ContentSection, public RPCActionManager
{
 public:
  WebUISupport(UpdateDocumentContributor *master);

  void handleHTTPRequest(std::shared_ptr<NetworkRequest> request, const Glib::ustring &path) override;
  Glib::ustring getPrefix() const override;

 private:
  void writeDocument(Writer &writer, tUpdateID knownRevision) const override;
};
