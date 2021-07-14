#include <Application.h>
#include <http/HTTPRequest.h>
#include <http/ServedJournal.h>
#include <device-settings/DebugLevel.h>
#include <memory>
#include <giomm.h>
#include <CompileTimeOptions.h>

ServedJournal::ServedJournal(HTTPServer &server, std::shared_ptr<HTTPRequest> request)
    : ServedStream(server, request)
{
}

ServedJournal::~ServedJournal()
{
}

void ServedJournal::startServing()
{
  GError *error = nullptr;
  auto process = g_subprocess_new(G_SUBPROCESS_FLAGS_STDOUT_PIPE, &error, "journalctl", "-b0", nullptr);

  if(error)
  {
    DebugLevel::warning(error->message);
    g_error_free(error);
    sendNotFoundResponse();
  }
  else
  {
    GInputStream *output = g_subprocess_get_stdout_pipe(process);
    startResponseFromStream(Glib::wrap(output));
  }
}
