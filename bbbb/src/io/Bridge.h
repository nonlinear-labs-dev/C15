#pragma once

#include "Receiver.h"
#include <memory>

class Sender;
class Receiver;

class Bridge : public sigc::trackable
{
 public:
  Bridge(Sender *sender, Receiver *receiver);
  virtual ~Bridge();

 protected:
  virtual void transmit(Receiver::tMessage msg);

  std::unique_ptr<Sender> m_sender;
  std::unique_ptr<Receiver> m_receiver;
};
