#pragma once

#include <http/ServedStream.h>

class ServedJournal : public ServedStream
{
 public:
  ServedJournal(HTTPServer &server, std::shared_ptr<HTTPRequest> request);
  ~ServedJournal() override;

  void startServing() override;
};
