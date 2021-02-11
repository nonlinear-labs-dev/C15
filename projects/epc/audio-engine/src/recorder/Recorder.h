#pragma once

#include "Types.h"
#include <nlohmann/json.hpp>
#include <memory>

class FlacFrameStorage;
class RecorderInput;
class RecorderOutput;

namespace nltools
{
  namespace msg
  {
    class WebSocketJsonAPI;
  }
}

class Recorder
{
 public:
  Recorder(int sr);
  ~Recorder();

  void process(SampleFrame* target, size_t numFrames);

  RecorderInput* getInput() const;
  RecorderOutput* getOutput() const;
  FlacFrameStorage* getStorage() const;

 private:
  nlohmann::json api(const nlohmann::json& msg);
  nlohmann::json generateInfo() const;
  nlohmann::json queryFrames(FrameId begin, FrameId end) const;

  std::unique_ptr<FlacFrameStorage> m_storage;
  std::unique_ptr<RecorderInput> m_in;
  std::unique_ptr<RecorderOutput> m_out;
  std::unique_ptr<nltools::msg::WebSocketJsonAPI> m_api;
};
