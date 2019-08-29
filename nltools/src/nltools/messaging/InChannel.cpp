#include <iostream>
#include <utility>
#include "InChannel.h"

namespace nltools
{
  namespace msg
  {
    InChannel::InChannel(Callback cb)
        : m_cb(std::move(cb))
    {
    }

    InChannel::~InChannel() = default;

    void InChannel::onMessageReceived(const SerializedMessage &msg) const
    {
      std::cerr << "InChannel::onMessageReceived" << std::endl;
      m_cb(msg);
    }
  }
}