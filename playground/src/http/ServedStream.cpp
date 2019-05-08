#include "ServedStream.h"
#include "HTTPServer.h"
#include "HTTPRequest.h"
#include "Application.h"
#include <device-settings/DebugLevel.h>

ServedStream::ServedStream(HTTPServer &server, shared_ptr<HTTPRequest> request)
    : m_server(server)
    , m_request(request)
{
  m_cancellable = Gio::Cancellable::create();
}

ServedStream::~ServedStream()
{
  m_cancellable->cancel();
}

void ServedStream::sendNotFoundResponse()
{
  m_request->notFound();
}

void ServedStream::serveStream(const Glib::RefPtr<Gio::InputStream> &stream)
{
  if(stream)
    startResponseFromStream(stream);
  else
    sendNotFoundResponse();
}

void ServedStream::startResponseFromStream(Glib::RefPtr<Gio::InputStream> stream)
{
  doAsyncReadFromStream(stream);
  m_request->setStatusOK();
  m_request->setChunkedEncoding();
}

gsize ServedStream::writeReadBytesToResponse(gsize numBytes)
{
  m_request->respond(m_readBuffer, numBytes);
  return numBytes;
}

void ServedStream::doAsyncReadFromStream(RefPtr<Gio::InputStream> stream)
{
  stream->read_async(m_readBuffer, sizeof(m_readBuffer),
                     sigc::bind(mem_fun(this, &ServedStream::onAsyncStreamRead), stream), m_cancellable);
}

void ServedStream::onAsyncStreamRead(RefPtr<Gio::AsyncResult> res, RefPtr<Gio::InputStream> stream)
{
  try
  {
    gsize numBytes = stream->read_finish(res);
    gsize bytesWritten = writeReadBytesToResponse(numBytes);

    if(bytesWritten > 0)
      doAsyncReadFromStream(stream);
    else
      m_request->complete();
  }
  catch(...)
  {
    DebugLevel::error("Could not serve stream");
  }
}

bool ServedStream::matches(SoupMessage *msg) const
{
  return m_request->matches(msg);
}
