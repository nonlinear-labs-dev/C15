#pragma once

#include "playground.h"

class Sender
{
 public:
  using tMessage = Glib::RefPtr<Glib::Bytes>;

  Sender();
  virtual ~Sender();

  virtual void send(tMessage msg) = 0;
};
