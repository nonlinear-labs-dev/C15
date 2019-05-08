#pragma once

#include "playground.h"

class MessageComposer
{
 public:
  MessageComposer(uint16_t type);
  virtual ~MessageComposer();

  void operator<<(const uint16_t &value);
  void operator<<(const gint16 &value);

  typedef Glib::Bytes tBuffer;
  typedef Glib::RefPtr<tBuffer> tBufferPtr;
  virtual tBufferPtr flush();

  virtual bool canReplace(const MessageComposer *other) const;

 private:
  RefPtr<Gio::DataOutputStream> m_stream;
};
