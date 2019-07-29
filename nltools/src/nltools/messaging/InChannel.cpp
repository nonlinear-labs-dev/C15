#include "InChannel.h"

namespace nltools::msg
{
  InChannel::InChannel(Callback cb)
      : m_cb(cb)
  {
  }

  InChannel::~InChannel() = default;

  void InChannel::onMessageReceived(const SerializedMessage &msg) const
  {
    m_cb(msg);
  }
}
