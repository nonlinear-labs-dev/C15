#pragma once

#include "playground.h"

class Receiver : public sigc::trackable
{
 public:
  using tMessage = Glib::RefPtr<Glib::Bytes>;
  using Callback = std::function<void(tMessage)>;

  Receiver();
  virtual ~Receiver();

  void setCallback(Callback cb);

 protected:
  virtual void onDataReceived(Glib::RefPtr<Glib::Bytes> bytes);

 private:
  Callback m_callback;
};
