#pragma once

#include "playground.h"
#include "UpdateDocumentContributor.h"
#include <libsoup/soup.h>

class Writer;
class NetworkRequest;

class ContentSection : public UpdateDocumentContributor
{
 public:
  explicit ContentSection(UpdateDocumentContributor *parent);
  ~ContentSection() override;

  /*! return the prefix for routing requests to this section */
  [[nodiscard]] virtual Glib::ustring getPrefix() const = 0;

  /*! write the xml to the message */
  virtual void handleHTTPRequest(std::shared_ptr<NetworkRequest> request, const Glib::ustring &path);

  [[nodiscard]] bool contains(const Glib::ustring &path) const;
};
