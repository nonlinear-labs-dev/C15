#include <io/files/FileIOSender.h>
#include <nltools/logging/Log.h>

FileIOSender::FileIOSender(const char *path)
    : m_path(path)
{
  nltools::Log::notify("open file for writing: ", path);

  try
  {
    m_channel = Glib::IOChannel::create_from_file(path, "w+");
    m_channel->set_encoding();
  }
  catch(...)
  {
    nltools::Log::error("could not open file for writing: ", path);
  }
}

FileIOSender::~FileIOSender()
{
}

void FileIOSender::send(tMessage msg)
{
  gsize len = 0;
  auto data = reinterpret_cast<const char *>(msg->get_data(len));
  write(data, len);
}

void FileIOSender::write(const char *bytes, size_t numBytes)
{
  if(m_channel)
  {
    try
    {
      gsize numBytesWritten = 0;
      m_channel->write(bytes, static_cast<gssize>(numBytes), numBytesWritten);
      m_channel->flush();
    }
    catch(Glib::Error &err)
    {
      nltools::Log::error("Exception: ", m_path, " -> ", err.what());
    }
  }
}
