#pragma once

#include "playground.h"
#include <libsoup/soup.h>

class HTTPServer;
class HTTPRequest;

class ServedStream
{
 public:
  ServedStream(HTTPServer &server, shared_ptr<HTTPRequest> request);
  virtual ~ServedStream();

  virtual void startServing() = 0;
  bool matches(SoupMessage *msg) const;

 protected:
  void sendNotFoundResponse();
  void startResponseFromStream(Glib::RefPtr<Gio::InputStream> stream);

  shared_ptr<HTTPRequest> m_request;
  RefPtr<Gio::Cancellable> m_cancellable;

 private:
  gsize writeReadBytesToResponse(gsize numBytes);
  void doAsyncReadFromStream(RefPtr<Gio::InputStream> stream);
  void onAsyncStreamRead(RefPtr<Gio::AsyncResult> res, RefPtr<Gio::InputStream> stream);
  void serveStream(const Glib::RefPtr<Gio::InputStream> &stream);

  HTTPServer &m_server;
  guint8 m_readBuffer[1024];
};
