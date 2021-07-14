#pragma once

#include "playground.h"
#include "ServedStream.h"
#include <libsoup/soup.h>

class HTTPServer;
class HTTPRequest;

class ServedFile : public ServedStream
{
 public:
  ServedFile(HTTPServer &server, std::shared_ptr<HTTPRequest> request);
  ~ServedFile() override;

  void startServing() override;

 private:
  [[nodiscard]] Glib::ustring getFilePathFromMessagePath() const;
  void startServingLocalFile(Glib::RefPtr<Gio::File> file);
  void setFilesContentType(Glib::RefPtr<Gio::File> file);
  void onAsyncFileRead(Glib::RefPtr<Gio::AsyncResult> res, Glib::RefPtr<Gio::File> file);
};
