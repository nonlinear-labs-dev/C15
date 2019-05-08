#pragma once

#include "playground.h"
#include "UpdateDocumentContributor.h"
#include <libsoup/soup.h>

class Writer;
class NetworkRequest;

class ContentSection : public UpdateDocumentContributor
{
 public:
  ContentSection(UpdateDocumentContributor *parent);
  virtual ~ContentSection();

  /*! return the prefix for routing requests to this section */
  virtual Glib::ustring getPrefix() const = 0;

  /*! write the xml to the message */
  virtual void handleHTTPRequest(shared_ptr<NetworkRequest> request, const Glib::ustring &path);

  bool contains(const Glib::ustring &path) const;
};
