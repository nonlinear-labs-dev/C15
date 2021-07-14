#pragma once

#include "playground.h"
#include "ContentManager.h"
#include "MCViewContentManager.h"
#include <libsoup/soup.h>
#include <map>
#include <memory>

class Application;
class ServedStream;
class NetworkRequest;
class HTTPRequest;
class MacroControlParameter;

class HTTPServer
{
 public:
  HTTPServer();
  virtual ~HTTPServer();

  void init();

  virtual void handleRequest(std::shared_ptr<NetworkRequest> request);
  virtual void onMessageFinished(SoupMessage *msg);

  void pauseMessage(SoupMessage *msg);
  void unpauseMessage(SoupMessage *msg);

  UpdateDocumentMaster *getUpdateDocumentMaster();
  const MCViewContentManager &getMCViewContentManager() const;

 private:
  static void serverCallback(SoupServer *server, SoupMessage *msg, const char *path, GHashTable *query,
                             SoupClientContext *, HTTPServer *pThis);
  static void webSocket(SoupServer *, SoupWebsocketConnection *connection, const char *, SoupClientContext *,
                        HTTPServer *pThis);

  static void mcWebSocket(SoupServer *, SoupWebsocketConnection *connection, const char *, SoupClientContext *,
                          HTTPServer *pThis);

  static void messageFinishedCB(SoupMessage *msg, HTTPServer *pThis);

  void startServer();
  void initializeServer();

  void serveStaticFile(std::shared_ptr<HTTPRequest> request);

  static bool isIndexPageAlias(const Glib::ustring &path);
  static Glib::ustring getPathFromMessage(SoupMessage *msg);
  void redirectToIndexPage(std::shared_ptr<HTTPRequest> request) const;

  static bool isStaticFileURL(const Glib::ustring &path);

  SoupServer *m_server = nullptr;
  ContentManager m_contentManager;
  MCViewContentManager m_mcviewManager;

  typedef std::shared_ptr<ServedStream> tServedStream;
  typedef std::shared_ptr<HTTPRequest> tHTTPRequest;
  std::list<tServedStream> m_servedStreams;
};
