#include "ServedFile.h"
#include "HTTPServer.h"
#include "HTTPRequest.h"
#include "Application.h"
#include <device-settings/DebugLevel.h>

ServedFile::ServedFile(HTTPServer &server, shared_ptr<HTTPRequest> request)
    : ServedStream(server, request)
{
}

ServedFile::~ServedFile()
{
}

void ServedFile::startServing()
{
  Glib::ustring filePath = getFilePathFromMessagePath();
  RefPtr<Gio::File> file = Gio::File::create_for_path(filePath);

  if(file->query_exists())
    startServingLocalFile(file);
  else
    sendNotFoundResponse();
}

Glib::ustring ServedFile::getFilePathFromMessagePath() const
{
  Glib::ustring p = Application::get().getSelfPath();
  size_t lastSlash = p.rfind('/');
  Glib::ustring path = m_request->getPath();
  p = p.substr(0, lastSlash) + path;
  return p;
}

void ServedFile::startServingLocalFile(RefPtr<Gio::File> file)
{
  setFilesContentType(file);
  file->read_async(sigc::bind(sigc::mem_fun(this, &ServedFile::onAsyncFileRead), file), m_cancellable);
  m_request->pause();
}

void ServedFile::setFilesContentType(RefPtr<Gio::File> file)
{
  RefPtr<Gio::FileInfo> info = file->query_info();
  Glib::ustring contentType = info->get_content_type();
  m_request->setContentType(contentType);
}

void ServedFile::onAsyncFileRead(RefPtr<Gio::AsyncResult> res, RefPtr<Gio::File> file)
{
  try
  {
    Glib::RefPtr<Gio::FileInputStream> stream = file->read_finish(res);
    startResponseFromStream(stream);
  }
  catch(...)
  {
    DebugLevel::error("Could not serve file");
  }
}
