#pragma once

#include <Types.h>
#include <FLAC/stream_encoder.h>
#include <vector>
#include <chrono>
#include <functional>
#include <memory>
#include <nltools/nlohmann/json.hpp>

class FlacEncoder
{
 public:
  constexpr static auto flacFrameSize = 4096;

  struct Frame
  {
    Frame(const FLAC__byte *buffer, size_t bytes, Sample max, std::chrono::system_clock::time_point timestamp);

    uint64_t getMemUsage() const;
    uint64_t getNextId();
    nlohmann::json generateInfo() const;

    std::vector<FLAC__byte> buffer;

    std::chrono::system_clock::time_point recordTime;
    uint8_t max;
    uint64_t id;
  };

  using CB = std::function<void(std::unique_ptr<Frame>, bool isHeader)>;

  FlacEncoder(int sr, CB cb);
  ~FlacEncoder();

  void push(const SampleFrame *frames, size_t numFrames);
  void resume();

 private:
  static FLAC__StreamEncoderWriteStatus writeToOut(const FLAC__StreamEncoder *, const FLAC__byte buffer[], size_t bytes,
                                                   uint32_t numSamples, uint32_t, void *client_data);

  FLAC__StreamEncoder *m_encoder = nullptr;
  Sample m_currentMax = 0;
  CB m_cb;

  std::chrono::system_clock::time_point m_resumedAt;
  size_t m_framesSinceResume = 0;
  bool m_writingHeader = false;
};
