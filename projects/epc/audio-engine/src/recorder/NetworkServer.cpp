#include "NetworkServer.h"
#include <libsoup/soup-message.h>
#include <nltools/logging/Log.h>
#include <iomanip>
#include <FLAC/stream_decoder.h>

namespace
{
  constexpr auto c_numChannels = 2;
  constexpr auto c_rate = 48000;

  struct WaveHeader
  {
    constexpr static size_t bytesPerSample = 3;
    constexpr static size_t bytesPerFlacFrame = FlacEncoder::flacFrameSize * c_numChannels * bytesPerSample;
    constexpr static size_t maxNumFrames = std::numeric_limits<uint32_t>::max() / bytesPerFlacFrame;

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
    uint16_t formatTag = 1;  // PCM
    uint16_t channels = c_numChannels;
    uint32_t rate = c_rate;
    uint32_t bytesPerSecond = c_rate * c_numChannels * bytesPerSample;
    uint16_t blockAlign = c_numChannels * bytesPerSample;
    uint16_t bitsPerSample = bytesPerSample * 8;
    char data[4];
    uint32_t dataLength;
  };
}

struct NetworkServer::WaveStream::OnTheFlyDecoder
{
  FlacFrameStorage *m_storage;

  FLAC__StreamDecoder *m_decoder = nullptr;
  FlacFrameStorage::StreamHandle m_streamOfFrames;
  std::vector<uint8_t> m_readScratch;
  SoupServer *m_server;
  SoupMessage *m_msg;
  std::vector<std::unique_ptr<FlacEncoder::Frame> > m_header;
  bool m_written = false;
  size_t m_currentFrame = 0;

  OnTheFlyDecoder(FlacFrameStorage *s, FrameId begin, FrameId end, SoupServer *server, SoupMessage *msg)
      : m_storage(s)
      , m_decoder(FLAC__stream_decoder_new())
      , m_streamOfFrames(m_storage->startStream(begin, end))
      , m_server(server)
      , m_msg(msg)
      , m_header(m_streamOfFrames->getHeaders())
  {
    FLAC__stream_decoder_init_stream(m_decoder, &readCB, nullptr, nullptr, nullptr, nullptr, &writeCB, nullptr,
                                     &errorCB, this);
  }

  ~OnTheFlyDecoder()
  {
    FLAC__stream_decoder_delete(m_decoder);
  }

  bool process()
  {
    m_written = false;

    while(!m_written)
    {
      if(m_readScratch.empty() && m_streamOfFrames->eos())
        return false;

      if(FLAC__stream_decoder_get_state(m_decoder) == FLAC__STREAM_DECODER_OGG_ERROR)
      {
        nltools::Log::error(__PRETTY_FUNCTION__, __LINE__);
        return false;
      }

      if(FLAC__stream_decoder_get_state(m_decoder) == FLAC__STREAM_DECODER_END_OF_STREAM)
      {
        nltools::Log::error(__PRETTY_FUNCTION__, __LINE__);
        return false;
      }

      if(!FLAC__stream_decoder_process_single(m_decoder))
      {
        nltools::Log::error(__PRETTY_FUNCTION__, __LINE__);
        return false;
      }
    }
    return true;
  }

  static FLAC__StreamDecoderReadStatus readCB(const FLAC__StreamDecoder *decoder, FLAC__byte buffer[], size_t *bytes,
                                              void *client_data)
  {
    auto pThis = static_cast<OnTheFlyDecoder *>(client_data);

    if(!pThis->m_header.empty())
    {
      auto &f = pThis->m_header.front();
      std::copy(f->buffer.begin(), f->buffer.end(), buffer);
      *bytes = f->buffer.size();
      pThis->m_header.erase(pThis->m_header.begin());
      return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
    }

    if(pThis->m_readScratch.empty())
    {
      pThis->m_streamOfFrames->next([&](auto &h, auto) {
        pThis->m_readScratch.insert(pThis->m_readScratch.end(), h.buffer.begin(), h.buffer.end());
      });
    }

    if(!pThis->m_readScratch.empty())
    {
      auto todo = std::min(pThis->m_readScratch.size(), *bytes);
      std::copy(pThis->m_readScratch.begin(), pThis->m_readScratch.begin() + todo, buffer);
      pThis->m_readScratch.erase(pThis->m_readScratch.begin(), pThis->m_readScratch.begin() + todo);
      *bytes = todo;
      return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
    }

    return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
  }

  static FLAC__StreamDecoderWriteStatus writeCB(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame,
                                                const FLAC__int32 *const buffer[], void *client_data)
  {
    auto pThis = static_cast<OnTheFlyDecoder *>(client_data);
    pThis->m_written = true;

    auto left = buffer[0];
    auto right = buffer[1];

    struct __attribute__((packed)) U24
    {
      uint8_t a, b, c;
    };

    struct __attribute__((packed)) U24Frame
    {
      U24 left;
      U24 right;
    };

    std::vector<U24Frame> out(FlacEncoder::flacFrameSize);

    for(size_t i = 0; i < FlacEncoder::flacFrameSize; i++)
    {
      out[i].left.a = (left[i] >> 0) & 0xFF;
      out[i].left.b = (left[i] >> 8) & 0xFF;
      out[i].left.c = (left[i] >> 16) & 0xFF;

      out[i].right.a = (right[i] >> 0) & 0xFF;
      out[i].right.b = (right[i] >> 8) & 0xFF;
      out[i].right.c = (right[i] >> 16) & 0xFF;
    }

    soup_message_body_append(pThis->m_msg->response_body, SOUP_MEMORY_COPY, out.data(), out.size() * sizeof(U24Frame));
    soup_server_unpause_message(pThis->m_server, pThis->m_msg);

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
  }

  static void errorCB(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data)
  {
    nltools::Log::error(__LINE__, status);
  }
};

NetworkServer::WaveStream::WaveStream(SoupServer *server, SoupMessage *msg, FlacFrameStorage *storage, FrameId begin,
                                      FrameId end)
    : Stream(server, msg)
    , decoder(std::make_unique<OnTheFlyDecoder>(storage, begin, end, server, msg))
{
  auto maxEnd = begin + WaveHeader::maxNumFrames;
  end = std::min<size_t>(end, maxEnd);
  WaveHeader header(end - begin + 1);
  soup_message_body_append(msg->response_body, SOUP_MEMORY_COPY, &header, sizeof(WaveHeader));
  soup_server_unpause_message(server, msg);
}

void NetworkServer::WaveStream::onChunkWritten()
{
  process();
}

void NetworkServer::WaveStream::process()
{
  if(!decoder->process())
  {
    soup_message_body_complete(msg->response_body);
    soup_server_unpause_message(server, msg);
  }
}

NetworkServer::FlacStream::FlacStream(SoupServer *server, SoupMessage *msg, FlacFrameStorage *storage, FrameId begin,
                                      FrameId end)
    : Stream(server, msg)
    , stream(storage->startStream(begin, end))
{
  for(auto &h : stream->getHeaders())
    soup_message_body_append(msg->response_body, SOUP_MEMORY_COPY, h->buffer.data(), h->buffer.size());
}

void NetworkServer::FlacStream::onChunkWritten()
{
  auto append = [&](auto &h, auto) {
    soup_message_body_append(msg->response_body, SOUP_MEMORY_COPY, h.buffer.data(), h.buffer.size());
    soup_server_unpause_message(server, msg);
  };

  if(!stream->next(append))
    soup_message_body_complete(msg->response_body);
}

NetworkServer::Stream::Stream(SoupServer *server, SoupMessage *msg)
    : server(server)
    , msg(msg)
    , wroteChunkHandler(
          g_signal_connect(G_OBJECT(msg), "wrote-chunk", G_CALLBACK(&NetworkServer::Stream::onChunkWrittenCB), this))
{
}

NetworkServer::Stream::~Stream()
{
  g_signal_handler_disconnect(msg, wroteChunkHandler);
}

void NetworkServer::Stream::onChunkWrittenCB(SoupMessage *, Stream *pThis)
{
  pThis->onChunkWritten();
}

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

    if(g_str_has_suffix(path, ".wav"))
    {
      pThis->exportWave(msg, path, begin, end);
    }
    else if(g_str_has_suffix(path, ".flac"))
    {
      pThis->exportFlac(msg, path, begin, end);
    }
    else
    {
      soup_message_set_status(msg, SOUP_STATUS_NOT_FOUND);
      soup_message_body_complete(msg->response_body);
    }
  }
  else
  {
    soup_message_set_status(msg, SOUP_STATUS_MALFORMED);
    soup_message_body_complete(msg->response_body);
  }
}

void NetworkServer::exportWave(SoupMessage *msg, const std::string &path, FrameId begin, FrameId end)
{
  soup_message_set_status(msg, SOUP_KNOWN_STATUS_CODE_OK);
  soup_message_body_set_accumulate(msg->response_body, FALSE);
  soup_message_headers_set_encoding(msg->response_headers, SOUP_ENCODING_CHUNKED);
  soup_message_headers_set_content_type(msg->response_headers, "audio/wave", nullptr);
  soup_message_headers_set_content_disposition(msg->response_headers, "attachment", nullptr);

  g_signal_connect(G_OBJECT(msg), "finished", G_CALLBACK(&NetworkServer::onFinished), this);
  m_streams.push_back(std::make_unique<WaveStream>(m_server, msg, m_storage, begin, end));
}

void NetworkServer::exportFlac(SoupMessage *msg, const std::string &path, FrameId begin, FrameId end)
{
  soup_message_set_status(msg, SOUP_KNOWN_STATUS_CODE_OK);
  soup_message_body_set_accumulate(msg->response_body, FALSE);
  soup_message_headers_set_encoding(msg->response_headers, SOUP_ENCODING_CHUNKED);
  soup_message_headers_set_content_type(msg->response_headers, "audio/flac", nullptr);
  soup_message_headers_set_content_disposition(msg->response_headers, "attachment", nullptr);

  g_signal_connect(G_OBJECT(msg), "finished", G_CALLBACK(&NetworkServer::onFinished), this);
  m_streams.push_back(std::make_unique<FlacStream>(m_server, msg, m_storage, begin, end));
}

void NetworkServer::onFinished(SoupMessage *msg, NetworkServer *pThis)
{
  auto &streams = pThis->m_streams;
  auto e = std::remove_if(streams.begin(), streams.end(), [&](auto &s) { return s->msg == msg; });
  streams.erase(e, streams.end());
}
