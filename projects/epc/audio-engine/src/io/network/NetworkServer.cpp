#include "NetworkServer.h"
#include "FlacStream.h"
#include "OpusStream.h"
#include "WaveStream.h"
#include <libsoup/soup-message.h>
#include <algorithm>
#include <netinet/tcp.h>
#include <nltools/logging/Log.h>

NetworkServer::NetworkServer(const RingBuffer<SampleFrame> &ring, uint32_t sampleRate)
    : m_ring(ring)
    , m_sampleRate(sampleRate)
    , m_server(soup_server_new(nullptr, nullptr))
{
  soup_server_add_handler(m_server, nullptr, &NetworkServer::stream, this, nullptr);
  soup_server_listen_all(m_server, 9090, static_cast<SoupServerListenOptions>(0), nullptr);
}

NetworkServer::~NetworkServer()
{
  m_streams.clear();
  g_object_unref(m_server);
}

void NetworkServer::stream(SoupServer *server, SoupMessage *msg, const char *path, GHashTable *,
                           SoupClientContext *client, gpointer ctx)
{
  auto pThis = static_cast<NetworkServer *>(ctx);
  auto socket = soup_client_context_get_gsocket(client);

  GError *error = nullptr;

  auto cb = [=](auto data, auto numBytes) {
    soup_message_body_append(msg->response_body, SOUP_MEMORY_COPY, data, numBytes);
    soup_server_unpause_message(server, msg);
  };

  std::unique_ptr<EncodedStream> stream;

  if(g_str_has_suffix(path, "flac"))
  {
    stream = std::make_unique<FlacStream>(pThis->m_ring, pThis->m_sampleRate, cb);
  }
  else if(g_str_has_suffix(path, "opus"))
  {
    stream = std::make_unique<OpusStream>(pThis->m_ring, pThis->m_sampleRate, cb);
  }
  else if(g_str_has_suffix(path, "wav"))
  {
    stream = std::make_unique<WaveStream>(pThis->m_ring, pThis->m_sampleRate, cb);
  }
  else
  {
    soup_message_set_status(msg, SOUP_KNOWN_STATUS_CODE_NOT_FOUND);
    soup_message_finished(msg);
    return;
  }

  soup_message_set_status(msg, SOUP_KNOWN_STATUS_CODE_OK);
  soup_message_body_set_accumulate(msg->response_body, FALSE);
  soup_message_headers_set_encoding(msg->response_headers, SOUP_ENCODING_CHUNKED);
  soup_message_headers_set_content_type(msg->response_headers, stream->getContentType().c_str(), nullptr);

  g_signal_connect(G_OBJECT(msg), "finished", G_CALLBACK(&NetworkServer::onFinished), pThis);
  g_object_set_data(G_OBJECT(msg), "stream", stream.get());

  soup_server_pause_message(server, msg);
  pThis->m_streams.push_back(std::move(stream));
}

void NetworkServer::onFinished(SoupMessage *msg, NetworkServer *pThis)
{
  auto &streams = pThis->m_streams;
  auto stream = g_object_get_data(G_OBJECT(msg), "stream");
  auto e = std::remove_if(streams.begin(), streams.end(), [&](auto &s) { return s.get() == stream; });
  streams.erase(e, streams.end());
}
