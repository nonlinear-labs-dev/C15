#pragma once

#include <io/Sender.h>

class FileIOSender : public Sender
{
 public:
  FileIOSender(const char *path);
  virtual ~FileIOSender();

  void send(tMessage msg) override;
  void write(const char *bytes, size_t numBytes);

 private:
  Glib::RefPtr<Glib::IOChannel> m_channel;
  const char *m_path = nullptr;
};
