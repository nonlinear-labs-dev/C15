#pragma once

#include "NetworkRequest.h"
#include <libsoup/soup-message.h>

class Application;

class HTTPRequest : public NetworkRequest
{
 private:
  typedef NetworkRequest super;

 public:
  explicit HTTPRequest(SoupServer *server, SoupMessage *msg);
  ~HTTPRequest() override;

  std::unique_ptr<OutStream> createStream(const Glib::ustring &contentType, bool zip) override;

  void okAndComplete() override;
  void moved(const Glib::ustring &url);
  bool matches(SoupMessage *msg) const;

  Glib::ustring getPath() override;

  void pause() override;
  void unpause() override;
  void notFound();
  void setContentType(const Glib::ustring &contentType);
  void setStatusOK();
  void setChunkedEncoding();
  void respond(const uint8_t *data, gsize numBytes);
  void respond(const Glib::ustring &str);
  void respondComplete(uint status, const char *contentType,
                       const std::initializer_list<std::pair<std::string, std::string>> &headers,
                       std::vector<uint8_t> &&buffer);
  void complete();
  void setHeader(const Glib::ustring &key, const Glib::ustring &value);

  SoupBuffer *getFlattenedBuffer();

 private:
  SoupServer *m_server = nullptr;
  SoupMessage *m_message = nullptr;
};
