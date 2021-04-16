#include "Recorder.h"

#include <nltools/messaging/Messaging.h>
#include <nltools/messaging/Message.h>
#include <nltools/messaging/WebSocketJsonAPI.h>
#include <nltools/logging/Log.h>

#include "FlacFrameStorage.h"
#include "RecorderInput.h"
#include "RecorderOutput.h"
#include "NetworkServer.h"

constexpr int operator""_MB(unsigned long long const x)
{
  return 1024 * 1024 * x;
}

constexpr static auto c_flacFrameBufferSize = 500_MB;

Recorder::Recorder(int sr)
    : m_storage(std::make_unique<FlacFrameStorage>(c_flacFrameBufferSize))
    , m_in(std::make_unique<RecorderInput>(m_storage.get(), sr))
    , m_out(std::make_unique<RecorderOutput>(m_storage.get(), sr))
    , m_api(std::make_unique<nltools::msg::WebSocketJsonAPI>(RECORDER_WEBSOCKET_PORT,
                                                             [this](const auto &msg) { return api(msg); }))
    , m_http(std::make_unique<NetworkServer>(RECORDER_HTTPSERVER_PORT, m_storage.get()))
{
}

Recorder::~Recorder() = default;

void Recorder::process(SampleFrame *frames, size_t numFrames)
{
  m_in->process(frames, numFrames);
  m_out->process(frames, numFrames);
}

RecorderInput *Recorder::getInput() const
{
  return m_in.get();
}

RecorderOutput *Recorder::getOutput() const
{
  return m_out.get();
}

FlacFrameStorage *Recorder::getStorage() const
{
  return m_storage.get();
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
    else if(name == "set-playback-position")
      m_out->setPlayPos(args.at("frameId"));
    else if(name == "start-playback")
      m_out->start();
    else if(name == "pause-playback")
      m_out->pause();
    else if(name == "get-info")
      return generateInfo();
    else if(name == "query-frames")
      return queryFrames(args.at("begin"), args.at("end"));
    else if(name == "reset")
      m_storage->reset();
  }
  return {};
}

nlohmann::json Recorder::generateInfo() const
{
  nlohmann::json ret;
  ret["time"] = std::chrono::system_clock::now().time_since_epoch().count();
  ret["storage"] = m_storage->generateInfo();
  ret["recorder"] = m_in->generateInfo();
  ret["player"] = m_out->generateInfo();
  return ret;
}

nlohmann::json Recorder::queryFrames(FrameId begin, FrameId end) const
{
  constexpr std::chrono::nanoseconds frameLen(std::nano::den * FlacEncoder::flacFrameSize / 48000);

  auto ret = nlohmann::json::array();
  auto stream = m_storage->startStream(begin, end);

  constexpr auto invalidTime = std::chrono::system_clock::time_point::min();

  std::chrono::system_clock::time_point recordTimeOfLastFrame = invalidTime;
  uint8_t maxOfLastFrame = 0;

  auto cb = [&](const auto &f, auto isLast) {
    bool skipFrame = false;

    if(!isLast && recordTimeOfLastFrame != invalidTime)
    {
      if(f.max == maxOfLastFrame)
      {
        auto expectedTS = recordTimeOfLastFrame + frameLen;
        auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(expectedTS - f.recordTime);
        auto oneMS = 1000 * 1000;
        skipFrame = std::abs(diff.count()) < oneMS;
      }
    }

    if(!skipFrame)
      ret.push_back(f.generateInfo());

    recordTimeOfLastFrame = f.recordTime;
    maxOfLastFrame = f.max;
  };

  while(stream->next(cb))
    ;

  return ret;
}
