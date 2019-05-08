#pragma once

#include "playground.h"
#include "ServedStream.h"
#include <libsoup/soup.h>

class HTTPServer;
class HTTPRequest;

class ServedFile : public ServedStream
{
 public:
  ServedFile(HTTPServer &server, shared_ptr<HTTPRequest> request);
  virtual ~ServedFile();

  void startServing();

 private:
  Glib::ustring getFilePathFromMessagePath() const;
  void startServingLocalFile(RefPtr<Gio::File> file);
  void setFilesContentType(RefPtr<Gio::File> file);
  void onAsyncFileRead(RefPtr<Gio::AsyncResult> res, RefPtr<Gio::File> file);
};
