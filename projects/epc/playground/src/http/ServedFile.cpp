#include "ServedFile.h"
#include "HTTPServer.h"
#include "HTTPRequest.h"
#include "Application.h"
#include <device-settings/DebugLevel.h>
#include <giomm.h>
#include <CompileTimeOptions.h>

ServedFile::ServedFile(HTTPServer &server, std::shared_ptr<HTTPRequest> request)
    : ServedStream(server, request)
{
}

ServedFile::~ServedFile()
{
}

void ServedFile::startServing()
{
  Glib::ustring filePath;
  if(m_request->getPath().find("/tmp/") == 0)
    filePath = m_request->getPath();
  else
    filePath = getFilePathFromMessagePath();

  Glib::RefPtr<Gio::File> file = Gio::File::create_for_path(filePath);

  if(file->query_exists())
    startServingLocalFile(file);
  else
    sendNotFoundResponse();
}

Glib::ustring ServedFile::getFilePathFromMessagePath() const
{
  return getInstallDir() + "/" + m_request->getPath();
}

void ServedFile::startServingLocalFile(Glib::RefPtr<Gio::File> file)
{
  setFilesContentType(file);
  file->read_async(sigc::bind(sigc::mem_fun(this, &ServedFile::onAsyncFileRead), file), m_cancellable);
  m_request->pause();
}

void ServedFile::setFilesContentType(Glib::RefPtr<Gio::File> file)
{
  Glib::RefPtr<Gio::FileInfo> info = file->query_info();
  Glib::ustring contentType = info->get_content_type();
  m_request->setContentType(contentType);
}

void ServedFile::onAsyncFileRead(Glib::RefPtr<Gio::AsyncResult> res, Glib::RefPtr<Gio::File> file)
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
