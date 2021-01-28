#include "FlacFrameStorage.h"
#include <nltools/logging/Log.h>

FlacFrameStorage::FlacFrameStorage(uint64_t maxMemUsage)
    : m_maxMemUsage(maxMemUsage)
{
}

void FlacFrameStorage::push(std::unique_ptr<FlacEncoder::Frame> frame)
{
  std::unique_lock<std::mutex> l(m_mutex);

  if(m_header.size() < 3)
  {
    m_header.push_back(std::move(frame));
    return;
  }

  m_streams.erase(std::remove_if(m_streams.begin(), m_streams.end(), [](auto &s) { return s.unique(); }),
                  m_streams.end());

  m_memUsage += frame->getMemUsage();
  m_frames.push_back(std::move(frame));

  while(m_memUsage > m_maxMemUsage)
  {
    auto invalid = m_frames.begin();

    for(auto &s : m_streams)
    {
      if(s->it == invalid)
        s->it = std::next(invalid);

      if(s->end == invalid)
        s->end = std::next(invalid);
    }

    m_memUsage -= (*invalid)->getMemUsage();
    m_frames.pop_front();
  }
}

FlacFrameStorage::StreamHandle FlacFrameStorage::startStream(FrameId begin, FrameId end)
{
  std::unique_lock<std::mutex> l(m_mutex);

  struct Cmp
  {
    bool operator()(const std::unique_ptr<FlacEncoder::Frame> &frame, FrameId id) const
    {
      return frame->id < id;
    }

    bool operator()(FrameId id, const std::unique_ptr<FlacEncoder::Frame> &frame) const
    {
      return id < frame->id;
    }
  };

  auto beginIt = std::lower_bound(m_frames.begin(), m_frames.end(), begin, Cmp {});
  auto endIt = std::upper_bound(m_frames.begin(), m_frames.end(), end, Cmp {});

  if(end <= begin)
    endIt = beginIt;

  auto stream = std::make_shared<Stream>(this, beginIt, endIt);
  m_streams.push_back(stream);
  return m_streams.back();
}

nlohmann::json FlacFrameStorage::generateInfo()
{
  std::unique_lock<std::mutex> l(m_mutex);

  if(m_frames.empty())
    return {};

  nlohmann::json memUsage = { { "current", m_memUsage }, { "max", m_maxMemUsage } };

  return { { "mem-usage", memUsage },
           { "first", m_frames.front()->generateInfo() },
           { "last", m_frames.back()->generateInfo() } };
}

const FlacFrameStorage::Frames &FlacFrameStorage::getFrames() const
{
  return m_frames;
}

const std::vector<std::unique_ptr<FlacEncoder::Frame> > &FlacFrameStorage::getHeaders() const
{
  std::unique_lock<std::mutex> l(m_mutex);
  return m_header;
}

FlacFrameStorage::Stream::Stream(FlacFrameStorage *s, Frames::const_iterator begin, Frames::const_iterator end)
    : storage(s)
    , it(begin)
    , end(end)
{
}

bool FlacFrameStorage::Stream::eos() const
{
  std::unique_lock<std::mutex> l(storage->m_mutex);
  return storage->m_frames.end() == it || it == end;
}

bool FlacFrameStorage::Stream::next(std::function<void(const FlacEncoder::Frame &)> cb)
{
  std::unique_lock<std::mutex> l(storage->m_mutex);

  if(it == end)
    return false;

  cb(*(it++)->get());
  return true;
}
