#include "StreamServer.h"
#include "FlacStream.h"
#include <libsoup/soup-message.h>
#include <algorithm>
#include <netinet/tcp.h>
#include <nltools/logging/Log.h>

StreamServer::StreamServer(const Synth *synth)
    : m_synth(synth)
    , m_server(soup_server_new(nullptr, nullptr))
{
  soup_server_add_handler(m_server, "/flac", &StreamServer::flac, this, nullptr);
  soup_server_listen_all(m_server, 9090, static_cast<SoupServerListenOptions>(0), nullptr);
}

StreamServer::~StreamServer()
{
  m_streams.clear();
  g_object_unref(m_server);
}

void StreamServer::flac(SoupServer *server, SoupMessage *msg, const char *, GHashTable *, SoupClientContext *client,
                        gpointer ctx)
{
  auto pThis = static_cast<StreamServer *>(ctx);
  auto socket = soup_client_context_get_gsocket(client);

  nltools::Log::warning("NODELAY:", g_socket_set_option(socket, SOL_TCP, TCP_NODELAY, 1, nullptr));
  nltools::Log::warning("CORK:", g_socket_set_option(socket, SOL_TCP, TCP_CORK, 0, nullptr));

  soup_message_set_status(msg, SOUP_KNOWN_STATUS_CODE_OK);
  soup_message_body_set_accumulate(msg->response_body, FALSE);
  soup_message_headers_set_encoding(msg->response_headers, SOUP_ENCODING_CHUNKED);
  soup_message_headers_set_content_type(msg->response_headers, "audio/x-flac", nullptr);

  auto stream = std::make_unique<FlacStream>(pThis->m_synth, [=](auto data, auto numBytes) {
    soup_message_body_append(msg->response_body, SOUP_MEMORY_COPY, data, numBytes);
    soup_server_unpause_message(server, msg);
  });

  g_signal_connect(G_OBJECT(msg), "finished", G_CALLBACK(&StreamServer::onFinished), pThis);
  g_object_set_data(G_OBJECT(msg), "stream", stream.get());

  pThis->m_streams.push_back(std::move(stream));
}

void StreamServer::onFinished(SoupMessage *msg, StreamServer *pThis)
{
  auto &streams = pThis->m_streams;
  auto stream = g_object_get_data(G_OBJECT(msg), "stream");
  auto e = std::remove_if(streams.begin(), streams.end(), [&](auto &s) { return s.get() == stream; });
  streams.erase(e, streams.end());
}
