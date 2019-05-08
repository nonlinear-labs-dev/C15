#pragma once

#include "NetworkRequest.h"

class Application;

class HTTPRequest : public NetworkRequest
{
 private:
  typedef NetworkRequest super;

 public:
  HTTPRequest(SoupMessage *msg);
  virtual ~HTTPRequest();

  virtual shared_ptr<OutStream> createStream(const Glib::ustring &contentType, bool zip) override;

  void okAndComplete() override;
  void moved(const Glib::ustring &url);
  bool matches(SoupMessage *msg) const;

  Glib::ustring getPath() override;

  virtual void pause() override;
  virtual void unpause() override;
  void notFound();
  void setContentType(const Glib::ustring &contentType);
  void setStatusOK();
  void setChunkedEncoding();
  void respond(const uint8_t *data, gsize numBytes);
  void respond(const Glib::ustring &str);
  void complete();
  void setHeader(const Glib::ustring &key, const Glib::ustring &value);

  SoupBuffer *getFlattenedBuffer();

 private:
  SoupMessage *m_message = nullptr;
};
