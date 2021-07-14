#pragma once

#include "FlacEncoder.h"
#include <list>
#include <mutex>
#include <nltools/nlohmann/json.hpp>

class FlacFrameStorage
{
 public:
  using FramePtr = std::unique_ptr<FlacEncoder::Frame>;
  using Frames = std::list<FramePtr>;

  struct Stream
  {
    Stream(FlacFrameStorage *s, Frames::const_iterator begin, Frames::const_iterator end);

    bool eos() const;
    bool next(std::function<void(FlacEncoder::Frame &, bool last)> cb);

    bool getFirstAndLast(std::function<void(const FlacEncoder::Frame &first, const FlacEncoder::Frame &last)> cb);
    std::vector<std::unique_ptr<FlacEncoder::Frame>> getHeaders() const;

   private:
    friend class FlacFrameStorage;
    size_t frameNumber = 0;
    FlacFrameStorage *storage;
    Frames::const_iterator it;
    Frames::const_iterator end;
  };

  using StreamHandle = std::shared_ptr<Stream>;

  FlacFrameStorage(uint64_t m_maxMemUsage);

  void push(std::unique_ptr<FlacEncoder::Frame> frame, bool isHeader);
  const std::vector<std::unique_ptr<FlacEncoder::Frame>> &getHeaders() const;
  nlohmann::json reset();

  StreamHandle startStream(FrameId begin, FrameId end);
  nlohmann::json generateInfo();

  const Frames &getFrames() const;

 private:
  Frames m_frames;
  std::vector<StreamHandle> m_streams;
  std::vector<std::unique_ptr<FlacEncoder::Frame>> m_header;

  uint64_t m_memUsage = 0;
  uint64_t m_maxMemUsage = 0;
  mutable std::mutex m_mutex;
};