#pragma once
#include <http/NetworkRequest.h>

class WebSocketRequest : public NetworkRequest
{
 private:
  typedef NetworkRequest super;

 public:
  WebSocketRequest(SoupWebsocketConnection *socket, GBytes *message);
  virtual ~WebSocketRequest();

  virtual std::shared_ptr<OutStream> createStream(const Glib::ustring &contentType, bool zip);
  virtual void okAndComplete();
  virtual void unpause();
  virtual void pause();
  virtual Glib::ustring getPath();

  const SoupWebsocketConnection *getSocket() const;

 private:
  SoupWebsocketConnection *m_socket = nullptr;
  Glib::ustring m_path;
};
