#pragma once

#include "playground.h"
#include <glibmm/refptr.h>

class MessageComposer
{
 public:
  explicit MessageComposer(uint16_t type);
  virtual ~MessageComposer();

  void operator<<(const uint16_t &value);
  void operator<<(const gint16 &value);

  typedef Glib::Bytes tBuffer;
  typedef Glib::RefPtr<tBuffer> tBufferPtr;
  virtual tBufferPtr flush();

 private:
  Glib::RefPtr<Gio::DataOutputStream> m_stream;
};
