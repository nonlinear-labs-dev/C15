#include "HTTPRequest.h"
#include "SoupOutStream.h"
#include "Application.h"
#include "HTTPServer.h"
#include <iostream>

HTTPRequest::HTTPRequest(SoupMessage *msg)
    : m_message(msg)
{
  g_object_ref(m_message);

  SoupURI *uri = soup_message_get_uri(msg);
  const char *queryString = soup_uri_get_query(uri);

  if(queryString)
  {
    createMapFromQueryString(queryString);
  }
  else
  {
    SoupBuffer *buffer = soup_message_body_flatten(msg->request_body);

    if(buffer)
    {
      auto header = soup_message_headers_get_one(msg->request_headers, "Content-Type");
      bool containsBlob = header && (g_strcmp0(header, "application/binary") == 0);

      if(!containsBlob)
      {
        if(buffer->length < 10 * 1024 * 1024)
        {
          createMapFromQueryString(buffer->data);
        }
      }

      soup_buffer_free(buffer);
    }
  }
}

HTTPRequest::~HTTPRequest()
{
  g_object_unref(m_message);
}

Glib::ustring HTTPRequest::getPath()
{
  SoupURI *uri = soup_message_get_uri(m_message);
  return soup_uri_get_path(uri);
}

std::shared_ptr<OutStream> HTTPRequest::createStream(const Glib::ustring &contentType, bool zip)
{
  return std::shared_ptr<OutStream>(new SoupOutStream(m_message, contentType, zip));
}

void HTTPRequest::pause()
{
  Application::get().getHTTPServer()->pauseMessage(m_message);
}

void HTTPRequest::unpause()
{
  Application::get().getHTTPServer()->unpauseMessage(m_message);
}

void HTTPRequest::setContentType(const Glib::ustring &contentType)
{
  soup_message_headers_append(m_message->response_headers, "Content-Type", contentType.c_str());
}

void HTTPRequest::okAndComplete()
{
  setStatusOK();
  complete();
}

void HTTPRequest::complete()
{
  soup_message_body_complete(m_message->response_body);
  unpause();
}

void HTTPRequest::setHeader(const Glib::ustring &key, const Glib::ustring &value)
{
  soup_message_headers_append(m_message->response_headers, key.c_str(), value.c_str());
}

void HTTPRequest::setStatusOK()
{
  soup_message_set_status(m_message, SOUP_STATUS_OK);
}

void HTTPRequest::respond(const uint8_t *data, gsize numBytes)
{
  soup_message_body_append(m_message->response_body, SOUP_MEMORY_COPY, data, numBytes);
  unpause();
}

void HTTPRequest::respond(const Glib::ustring &str)
{
  respond((const uint8_t *) str.c_str(), str.length());
}

void HTTPRequest::setChunkedEncoding()
{
  soup_message_headers_set_encoding(m_message->response_headers, SOUP_ENCODING_CHUNKED);
}

void HTTPRequest::notFound()
{
  soup_message_set_status(m_message, SOUP_STATUS_NOT_FOUND);
  soup_message_body_complete(m_message->response_body);
  unpause();
}

void HTTPRequest::moved(const Glib::ustring &url)
{
  soup_message_set_status(m_message, SOUP_STATUS_MOVED_PERMANENTLY);
  soup_message_headers_append(m_message->response_headers, "Location", url.c_str());
  soup_message_body_complete(m_message->response_body);
}

bool HTTPRequest::matches(SoupMessage *msg) const
{
  return msg == m_message;
}

SoupBuffer *HTTPRequest::getFlattenedBuffer()
{
  return soup_message_body_flatten(m_message->request_body);
}
