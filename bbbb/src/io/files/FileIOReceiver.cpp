#include <io/files/FileIOReceiver.h>
#include <nltools/logging/Log.h>

FileIOReceiver::FileIOReceiver(const char *path, size_t blockSize)
    : m_cancel(Gio::Cancellable::create())
    , m_blockSize(blockSize)
{
  nltools::Log::notify("open file for reading: ", path);
  auto file = Gio::File::create_for_path(path);
  file->read_async(sigc::bind(sigc::mem_fun(this, &FileIOReceiver::onFileOpened), file), m_cancel);
}

FileIOReceiver::~FileIOReceiver()
{
  m_cancel->cancel();
}

void FileIOReceiver::onFileOpened(Glib::RefPtr<Gio::AsyncResult> &result, Glib::RefPtr<Gio::File> file)
{
  try
  {
    readStream(Gio::BufferedInputStream::create(file->read_finish(result)));
  }
  catch(...)
  {
    nltools::Log::error("Could not open file for reading: ", file->get_path());
  }
}

void FileIOReceiver::setBlockSize(size_t blockSize)
{
  m_blockSize = blockSize;
}

void FileIOReceiver::readStream(Glib::RefPtr<Gio::InputStream> stream)
{
  auto avail = Glib::RefPtr<Gio::BufferedInputStream>::cast_dynamic(stream)->get_available();

  if(avail >= m_blockSize)
  {
    auto bytes = stream->read_bytes(m_blockSize, m_cancel);
    onDataReceived(bytes);
    readStream(stream);
  }
  else
  {
    stream->read_bytes_async(m_blockSize, sigc::bind(sigc::mem_fun(this, &FileIOReceiver::onStreamRead), stream),
                             m_cancel, Glib::PRIORITY_HIGH);
  }
}

void FileIOReceiver::onStreamRead(Glib::RefPtr<Gio::AsyncResult> &result, Glib::RefPtr<Gio::InputStream> stream)
{
  auto bytes = stream->read_bytes_finish(result);
  onDataReceived(bytes);
  readStream(stream);
}
