#pragma once

#include "Types.h"
#include <libsoup/soup-server.h>
#include <vector>
#include <memory>

class EncodedStream;
template <typename T> class RingBuffer;

class NetworkServer
{
 public:
  NetworkServer(const RingBuffer<SampleFrame> &ring, uint32_t sampleRate);
  ~NetworkServer();

 private:
  static void stream(SoupServer *server, SoupMessage *msg, const char *, GHashTable *, SoupClientContext *client,
                     gpointer pThis);
  static void onFinished(SoupMessage *msg, NetworkServer *pThis);

  const RingBuffer<SampleFrame> &m_ring;
  uint32_t m_sampleRate;
  SoupServer *m_server = nullptr;
  std::vector<std::unique_ptr<EncodedStream>> m_streams;
};
