#include "NetworkServer.h"
#include <libsoup/soup-message.h>
#include <nltools/logging/Log.h>

NetworkServer::NetworkServer(int port, FlacFrameStorage *s)
    : m_storage(s)
    , m_server(soup_server_new(nullptr, nullptr))
{
  soup_server_add_handler(m_server, nullptr, &NetworkServer::stream, this, nullptr);
  soup_server_listen_all(m_server, port, static_cast<SoupServerListenOptions>(0), nullptr);
}

NetworkServer::~NetworkServer()
{
  m_streams.clear();
  g_object_unref(m_server);
}

void NetworkServer::stream(SoupServer *server, SoupMessage *msg, const char *path, GHashTable *query,
                           SoupClientContext *client, gpointer ctx)
{
  auto pThis = static_cast<NetworkServer *>(ctx);

  auto beginPtr = static_cast<const char *>(g_hash_table_lookup(query, "begin"));
  auto endPtr = static_cast<const char *>(g_hash_table_lookup(query, "end"));

  if(beginPtr && endPtr)
  {
    soup_message_set_status(msg, SOUP_KNOWN_STATUS_CODE_OK);
    soup_message_body_set_accumulate(msg->response_body, FALSE);
    soup_message_headers_set_encoding(msg->response_headers, SOUP_ENCODING_CHUNKED);
    soup_message_headers_set_content_type(msg->response_headers, "audio/x-flac", nullptr);

    g_signal_connect(G_OBJECT(msg), "finished", G_CALLBACK(&NetworkServer::onFinished), pThis);
    auto wroteChunkHandler
        = g_signal_connect(G_OBJECT(msg), "wrote-chunk", G_CALLBACK(&NetworkServer::onChunkWritten), pThis);

    for(auto &h : pThis->m_storage->getHeaders())
      soup_message_body_append(msg->response_body, SOUP_MEMORY_COPY, h->buffer.data(), h->buffer.size());

    auto begin = strtol(beginPtr, nullptr, 10);
    auto end = strtol(endPtr, nullptr, 10);
    pThis->m_streams.push_back({ msg, pThis->m_storage->startStream(begin, end), wroteChunkHandler });
  }
  else
  {
    soup_message_set_status(msg, SOUP_STATUS_MALFORMED);
  }
}

void NetworkServer::onFinished(SoupMessage *msg, NetworkServer *pThis)
{
  auto &streams = pThis->m_streams;
  auto e = std::remove_if(streams.begin(), streams.end(), [&](auto &s) { return s.msg == msg; });
  streams.erase(e, streams.end());
}

void NetworkServer::onChunkWritten(SoupMessage *msg, NetworkServer *pThis)
{
  for(auto &a : pThis->m_streams)
  {
    if(a.msg == msg)
    {
      auto append = [&](auto &h) {
        soup_message_body_append(msg->response_body, SOUP_MEMORY_COPY, h.buffer.data(), h.buffer.size());
      };

      if(!a.stream->next(append))
      {
        g_signal_handler_disconnect(msg, a.wroteChunkHandler);
        soup_message_body_complete(msg->response_body);
      }

      return;
    }
  }
}
