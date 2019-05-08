#pragma once

#include "playground.h"
#include "ContentManager.h"
#include <libsoup/soup.h>
#include <map>
#include <glib.h>
#include <memory>

class Application;
class ServedStream;
class AvahiService;
class NetworkRequest;
class HTTPRequest;

class HTTPServer
{
 public:
  HTTPServer();
  virtual ~HTTPServer();

  void init();

  virtual void handleRequest(shared_ptr<NetworkRequest> request);
  virtual void onMessageFinished(SoupMessage *msg);

  void pauseMessage(SoupMessage *msg);
  void unpauseMessage(SoupMessage *msg);

  UpdateDocumentMaster *getUpdateDocumentMaster();

 private:
  static void serverCallback(SoupServer *server, SoupMessage *msg, const char *path, GHashTable *query,
                             SoupClientContext *context, HTTPServer *pThis);
  static void webSocket(SoupServer *server, SoupWebsocketConnection *connection, const char *pathStr,
                        SoupClientContext *client, HTTPServer *pThis);

  static void messageFinishedCB(SoupMessage *msg, HTTPServer *pThis);

  void startServer();
  void initializeServer();

  void serveStaticFile(shared_ptr<HTTPRequest> request);

  static bool isIndexPageAlias(const Glib::ustring &path);
  static Glib::ustring getPathFromMessage(SoupMessage *msg);
  void redirectToIndexPage(shared_ptr<HTTPRequest> request) const;

  static bool isStaticFileURL(const Glib::ustring &path);
  void deliverJournal(std::shared_ptr<HTTPRequest> request) const;

  SoupServer *m_server = nullptr;
  ContentManager m_contentManager;

  typedef shared_ptr<ServedStream> tServedStream;
  typedef shared_ptr<HTTPRequest> tHTTPRequest;
  list<tServedStream> m_servedStreams;

  shared_ptr<AvahiService> m_avahi;
};
