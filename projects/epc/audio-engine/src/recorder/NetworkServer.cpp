#include "NetworkServer.h"
#include <libsoup/soup-message.h>
#include <nltools/logging/Log.h>
#include <iomanip>

namespace
{
  constexpr auto c_numChannels = 2;
  constexpr auto c_rate = 48000;

  struct WaveHeader
  {
    constexpr static auto bytesPerFlacFrame = FlacEncoder::flacFrameSize * c_numChannels * sizeof(float);
    constexpr static auto maxNumFrames = std::numeric_limits<uint32_t>::max() / bytesPerFlacFrame;

    WaveHeader(size_t numFlacFrames)
    {
      numFlacFrames = std::min(numFlacFrames, maxNumFrames);

      memcpy(riff, "RIFF", 4);
      memcpy(wave, "WAVE", 4);
      memcpy(fmt, "fmt ", 4);
      memcpy(data, "data", 4);
      dataLength = numFlacFrames * bytesPerFlacFrame;
      fileSize = dataLength + sizeof(WaveHeader) - 8;
    }

    char riff[4];
    uint32_t fileSize;
    char wave[4];
    char fmt[4];
    uint32_t fmtLength = 16;
    uint16_t formatTag = 3;  // floats
    uint16_t channels = c_numChannels;
    uint32_t rate = c_rate;
    uint32_t bytesPerSecond = c_rate * c_numChannels * sizeof(float);
    uint16_t blockAlign = c_numChannels * sizeof(float);
    uint16_t bitsPerSample = sizeof(float) * 8;
    char data[4];
    uint32_t dataLength;
  };
};

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
    auto begin = strtol(beginPtr, nullptr, 10);
    auto end = strtol(endPtr, nullptr, 10);

    if(strlen(path) <= 1)
    {
      auto stream = pThis->m_storage->startStream(begin, end);
      auto redirected = stream->getFirstAndLast([&](auto &first, auto &last) {
        auto format = "%a-%H-%M-%S";
        std::stringstream urlBuilder;
        urlBuilder << "/C15-Recording-";

        std::time_t tt = std::chrono::system_clock::to_time_t(first.recordTime);
        std::tm tm = *std::localtime(&tt);  //Locale time-zone, usually UTC by default.
        urlBuilder << std::put_time(&tm, format);
        urlBuilder << "-to-";

        tt = std::chrono::system_clock::to_time_t(last.recordTime);
        tm = *std::localtime(&tt);  //Locale time-zone, usually UTC by default.
        urlBuilder << std::put_time(&tm, format);
        urlBuilder << ".wav?begin=" << first.id << "&end=" << last.id;
        auto url = urlBuilder.str();
        soup_message_set_redirect(msg, SOUP_STATUS_MOVED_PERMANENTLY, url.c_str());
      });

      if(!redirected)
        soup_message_set_status(msg, SOUP_STATUS_NOT_FOUND);

      return;
    }

    soup_message_set_status(msg, SOUP_KNOWN_STATUS_CODE_OK);
    soup_message_body_set_accumulate(msg->response_body, FALSE);
    soup_message_headers_set_encoding(msg->response_headers, SOUP_ENCODING_CHUNKED);
    soup_message_headers_set_content_type(msg->response_headers, "audio/x-wav", nullptr);
    soup_message_headers_set_content_disposition(msg->response_headers, "attachment", nullptr);

    g_signal_connect(G_OBJECT(msg), "finished", G_CALLBACK(&NetworkServer::onFinished), pThis);
    auto wroteChunkHandler
        = g_signal_connect(G_OBJECT(msg), "wrote-chunk", G_CALLBACK(&NetworkServer::onChunkWritten), pThis);

    auto maxEnd = begin + WaveHeader::maxNumFrames;
    end = std::min<size_t>(end, maxEnd);
    WaveHeader header(end - begin);
    soup_message_body_append(msg->response_body, SOUP_MEMORY_COPY, &header, sizeof(WaveHeader));
    pThis->m_streams.push_back({ msg, std::make_unique<FlacDecoder>(pThis->m_storage, begin, end), wroteChunkHandler });
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
      if(a.decoder->eos())
      {
        g_signal_handler_disconnect(msg, a.wroteChunkHandler);
        soup_message_body_complete(msg->response_body);
      }

      SampleFrame target[FlacEncoder::flacFrameSize];
      auto avail = a.decoder->popAudio(target, FlacEncoder::flacFrameSize);
      soup_message_body_append(msg->response_body, SOUP_MEMORY_COPY, target, avail * sizeof(SampleFrame));
      return;
    }
  }
}
