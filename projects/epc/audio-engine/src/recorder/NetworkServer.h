#pragma once

#include <libsoup/soup-server.h>
#include "FlacFrameStorage.h"

class NetworkServer
{
 public:
  NetworkServer(int port, FlacFrameStorage *storage);
  ~NetworkServer();

 private:
  static void stream(SoupServer *server, SoupMessage *msg, const char *, GHashTable *, SoupClientContext *client,
                     gpointer pThis);
  static void onFinished(SoupMessage *msg, NetworkServer *pThis);
  static void onChunkWritten(SoupMessage *msg, NetworkServer *pThis);

  SoupServer *m_server = nullptr;
  FlacFrameStorage *m_storage = nullptr;

  struct Stream
  {
    SoupMessage *msg;
    FlacFrameStorage::StreamHandle stream;
    gulong wroteChunkHandler = 0;
  };

  std::vector<Stream> m_streams;
};
