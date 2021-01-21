#pragma once

#include <libsoup/soup-server.h>
#include <vector>
#include <memory>
#include "EncodedStream.h"

class EncodedStream;

class StreamServer
{
 public:
  StreamServer(const EncodedStream::AudioRing &ring, uint32_t sampleRate);
  ~StreamServer();

 private:
  static void stream(SoupServer *server, SoupMessage *msg, const char *, GHashTable *, SoupClientContext *client,
                   gpointer pThis);
  static void onFinished(SoupMessage *msg, StreamServer *pThis);

  const EncodedStream::AudioRing &m_ring;
  uint32_t m_sampleRate;
  SoupServer *m_server = nullptr;
  std::vector<std::unique_ptr<EncodedStream>> m_streams;
};
