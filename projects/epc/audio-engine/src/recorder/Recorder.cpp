#include "Recorder.h"

#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/Message.h>
#include <nltools/messaging/WebSocketJsonAPI.h>
#include <nltools/logging/Log.h>

#include "FlacFrameStorage.h"
#include "RecorderInput.h"
#include "RecorderOutput.h"

constexpr int operator""_MB(unsigned long long const x)
{
  return 1024 * 1024 * x;
}

constexpr static auto c_flacFrameBufferSize = 500_MB;

Recorder::Recorder(int sr)
    : m_storage(std::make_unique<FlacFrameStorage>(c_flacFrameBufferSize))
    , m_in(std::make_unique<RecorderInput>(m_storage.get(), sr))
    , m_out(std::make_unique<RecorderOutput>(m_storage.get(), sr))
    , m_api(std::make_unique<nltools::msg::WebSocketJsonAPI>(8889, [this](const auto &msg) { return api(msg); }))
{
}

Recorder::~Recorder() = default;

void Recorder::process(SampleFrame *frames, size_t numFrames)
{
  m_in->process(frames, numFrames);
  m_out->process(frames, numFrames);
}

nlohmann::json Recorder::api(const nlohmann::json &msg)
{
  if(msg.is_object())
  {
    auto obj = msg.get<nlohmann::json::object_t>();
    auto it = obj.begin();
    auto name = it->first;
    auto args = it->second;

    if(name == "toggle-recording")
      m_in->togglePause();
    else if(name == "start-playback")
      m_out->start(args.at("begin"), args.at("end"));
    else if(name == "pause-playback")
      m_out->pause();
    else if(name == "get-info")
      return generateInfo();
    else if(name == "get-waveform")
      return getWaveform(args.at("begin"), args.at("end"));
    else if(name == "query-frames")
      return getWaveform(args.at("begin"), args.at("end"));
  }
  return {};
}

nlohmann::json Recorder::generateInfo() const
{
  nlohmann::json ret;
  ret["storage"] = m_storage->generateInfo();
  ret["recorder"] = m_in->generateInfo();
  ret["player"] = m_out->generateInfo();
  return ret;
}

nlohmann::json Recorder::getWaveform(FrameId begin, FrameId end) const
{
  auto ret = nlohmann::json::array();
  auto stream = m_storage->startStream(begin, end);

  while(stream->next([&](const auto &f) { ret.push_back(f.max); }))
    ;
  return ret;
}

nlohmann::json Recorder::queryFrames(FrameId begin, FrameId end) const
{
  auto ret = nlohmann::json::array();
  auto stream = m_storage->startStream(begin, end);

  while(stream->next([&](const auto &f) { ret.push_back(f.generateInfo()); }))
    ;

  return ret;
}
