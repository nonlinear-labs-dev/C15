#pragma once

#include <memory>

class Sender;
class Receiver;

class Bridge
{
  public:
    Bridge(Sender *sender, Receiver *receiver);
    virtual ~Bridge();

  protected:
    std::unique_ptr<Sender> m_sender;
    std::unique_ptr<Receiver> m_receiver;
};

