#pragma once

#include <libsoup/soup-server.h>
#include <vector>
#include <memory>

class FlacStream;
class Synth;

class StreamServer
{
 public:
  StreamServer(const Synth *synth);
  ~StreamServer();

 private:
  static void flac(SoupServer *server, SoupMessage *msg, const char *, GHashTable *, SoupClientContext *client,
                   gpointer pThis);
  static void onFinished(SoupMessage *msg, StreamServer *pThis);

  const Synth *m_synth;
  SoupServer *m_server = nullptr;
  std::vector<std::unique_ptr<FlacStream>> m_streams;
};
