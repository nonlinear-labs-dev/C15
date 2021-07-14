#pragma once

#include <http/NetworkRequest.h>
#include <libsoup/soup.h>

class WebSocketRequest : public NetworkRequest
{
 private:
  typedef NetworkRequest super;

 public:
  WebSocketRequest(SoupWebsocketConnection *socket, GBytes *message);
  ~WebSocketRequest() override;

  std::unique_ptr<OutStream> createStream(const Glib::ustring &contentType, bool zip) override;
  void okAndComplete() override;
  void unpause() override;
  void pause() override;
  Glib::ustring getPath() override;

  [[nodiscard]] const SoupWebsocketConnection *getSocket() const;

 private:
  SoupWebsocketConnection *m_socket = nullptr;
  Glib::ustring m_path;
};
