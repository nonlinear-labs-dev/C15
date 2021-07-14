#include "HTTPRequest.h"
#include "SoupOutStream.h"

HTTPRequest::HTTPRequest(SoupServer *server, SoupMessage *msg)
    : m_server(server)
    , m_message(msg)
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

std::unique_ptr<OutStream> HTTPRequest::createStream(const Glib::ustring &contentType, bool zip)
{
  return std::make_unique<SoupOutStream>(m_server, m_message, contentType, zip);
}

void HTTPRequest::pause()
{
  soup_server_pause_message(m_server, m_message);
}

void HTTPRequest::unpause()
{
  soup_server_unpause_message(m_server, m_message);
}

void HTTPRequest::setContentType(const Glib::ustring &contentType)
{
  setHeader("Content-Type", contentType);
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

void HTTPRequest::respondComplete(uint status, const char *contentType,
                                  const std::initializer_list<std::pair<std::string, std::string> > &headers,
                                  std::vector<uint8_t> &&buffer)
{
  using Data = std::vector<uint8_t>;

  setStatusOK();
  setContentType(contentType);

  for(const auto &h : headers)
    setHeader(h.first, h.second);

  setHeader("Content-Length", std::to_string(buffer.size()));

  auto ptr = new Data(std::move(buffer));

  g_object_weak_ref(
      G_OBJECT(m_message), +[](gpointer data, GObject *) { delete static_cast<Data *>(data); }, ptr);
  soup_message_body_append(m_message->response_body, SOUP_MEMORY_TEMPORARY, ptr->data(), ptr->size());
  complete();
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
