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

  void exportWave(SoupMessage *msg, const std::string &path, FrameId begin, FrameId end);
  void exportFlac(SoupMessage *msg, const std::string &path, FrameId begin, FrameId end);

  SoupServer *m_server = nullptr;
  FlacFrameStorage *m_storage = nullptr;

  struct Stream
  {
   public:
    Stream(SoupServer *server, SoupMessage *msg);
    virtual ~Stream();

    SoupServer *server;
    SoupMessage *msg;

   private:
    virtual void onChunkWritten() = 0;
    static void onChunkWrittenCB(SoupMessage *, Stream *);

    gulong wroteChunkHandler = 0;
  };

  struct FlacStream : Stream
  {
    FlacStream(SoupServer *server, SoupMessage *msg, FlacFrameStorage *storage, FrameId begin, FrameId end);
    void onChunkWritten() override;

   private:
    FlacFrameStorage::StreamHandle stream;
    size_t currentFrame = 1;
  };

  struct WaveStream : Stream
  {
    struct OnTheFlyDecoder;
    WaveStream(SoupServer *server, SoupMessage *msg, FlacFrameStorage *storage, FrameId begin, FrameId end);
    void onChunkWritten() override;
    void process();
    std::unique_ptr<OnTheFlyDecoder> decoder;
  };

  std::vector<std::unique_ptr<Stream>> m_streams;
};
