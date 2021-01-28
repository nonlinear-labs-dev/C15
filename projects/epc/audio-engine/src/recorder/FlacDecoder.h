#pragma once

#include "FlacFrameStorage.h"
#include <FLAC/stream_decoder.h>
#include <tuple>

class FlacFrameStorage;

class FlacDecoder
{
 public:
  FlacDecoder(FlacFrameStorage *s, FrameId begin, FrameId end);
  ~FlacDecoder();

  using PositionInfo = std::tuple<FrameId, FrameId, FrameId>;

  PositionInfo getPositionInfo() const;
  size_t popAudio(SampleFrame *target, size_t maxNumFrames);

 private:
  static FLAC__StreamDecoderReadStatus readCB(const FLAC__StreamDecoder *m_decoder, FLAC__byte buffer[], size_t *bytes,
                                              void *client_data);

  static FLAC__StreamDecoderWriteStatus writeCB(const FLAC__StreamDecoder *m_decoder, const FLAC__Frame *frame,
                                                const FLAC__int32 *const buffer[], void *client_data);

  static void errorCB(const FLAC__StreamDecoder *m_decoder, FLAC__StreamDecoderErrorStatus status, void *client_data);

  FlacFrameStorage *m_storage;
  PositionInfo m_info;

  FLAC__StreamDecoder *m_decoder = nullptr;
  FlacFrameStorage::StreamHandle m_streamOfFrames;
  SampleFrame *m_currentTarget = nullptr;
  int m_headerPos = 0;
  std::vector<uint8_t> m_readScratch;
  bool m_audioDecoded = false;
};
