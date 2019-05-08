#pragma once

#include "playground.h"
#include "ContentSection.h"
#include "PendingHTTPRequests.h"
#include "UpdateDocumentMaster.h"
#include "UndoActions.h"
#include "UndoScope.h"
#include <set>
#include <chrono>

class WebSocketRequest;
class HTTPRequest;
class Application;
class ContentSection;

class ContentManager : public PendingHTTPRequests, public UpdateDocumentMaster, public sigc::trackable
{
 public:
  ContentManager();
  virtual ~ContentManager();

  void init();
  void handleRequest(std::shared_ptr<NetworkRequest> request);
  void onSectionMessageFinished(SoupMessage *msg);

  tUpdateID onChange(uint64_t flags = UpdateDocumentContributor::ChangeFlags::Generic) override;

  UNDO::Scope &getUndoScope() override;
  const UNDO::Scope &getUndoScope() const override;
  void connectWebSocket(SoupWebsocketConnection *connection);

  bool isSendResponsesScheduled() const;

  virtual void writeDocument(Writer &writer, tUpdateID knownRevision) const override;

 private:
  using tContentSectionPtr = ContentSection *;
  using tSections = std::set<tContentSectionPtr>;

  void addContentSections();
  void addContentSection(tContentSectionPtr section);
  void onSectionChanged();

  void deliverResponse(std::shared_ptr<HTTPRequest> request, tUpdateID clientsUpdateID);
  void deliverContentSectionResponse(ContentManager::tContentSectionPtr section,
                                     std::shared_ptr<NetworkRequest> request);
  void delayResponseUntilChanged(std::shared_ptr<HTTPRequest> request);

  bool tryHandlingContentSectionRequest(ContentManager::tContentSectionPtr section,
                                        std::shared_ptr<NetworkRequest> request);

  void writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision, bool omitOracles) const;
  void sendResponses();

  static void onWebSocketMessage(SoupWebsocketConnection *self, gint type, GBytes *message, ContentManager *pThis);

  tSections m_sections;

  bool m_sendResponsesScheduled = false;
  std::list<connection> m_connections;

  std::chrono::steady_clock::time_point m_lastUpdateSentAt;

  struct WebsocketConnection
  {
   public:
    WebsocketConnection(SoupWebsocketConnection *c);
    ~WebsocketConnection();

    void onWebsocketRequestDone(std::shared_ptr<WebSocketRequest> request, tUpdateID oldID, tUpdateID newId);
    SoupWebsocketConnection *getConnection();
    tUpdateID getLastSentUpdateId() const;
    void setLastSentUpdateId(int currentUpdateId);
    bool canOmitOracles(int currentUpdateId) const;

   private:
    SoupWebsocketConnection *ws;
    tUpdateID lastSentUpdateId;
    tUpdateID lastSelfIssuedUpdateId;
    bool allChangesWereOracles;
  };

  typedef std::shared_ptr<WebsocketConnection> tWebsocketConnection;

  void feedWebSockets();
  bool feedWebSocket(tWebsocketConnection ws);
  void onUpdateIdChangedByNetworkRequest(std::shared_ptr<NetworkRequest> request,
                                         UpdateDocumentContributor::tUpdateID oldUpdateID,
                                         UpdateDocumentContributor::tUpdateID newUpdateID);

  std::list<tWebsocketConnection> m_webSockets;
};
