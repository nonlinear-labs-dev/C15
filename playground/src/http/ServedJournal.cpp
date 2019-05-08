#include <Application.h>
#include <gio/gio.h>
#include <http/HTTPRequest.h>
#include <http/ServedJournal.h>
#include <device-settings/DebugLevel.h>
#include <memory>

ServedJournal::ServedJournal(HTTPServer &server, shared_ptr<HTTPRequest> request)
    : ServedStream(server, request)
{
}

ServedJournal::~ServedJournal()
{
}

void ServedJournal::startServing()
{
  auto command = Application::get().getResourcePath() + "pack-journal.sh";
  GError *error = nullptr;
  auto process = g_subprocess_new(G_SUBPROCESS_FLAGS_STDOUT_PIPE, &error, command.c_str(), nullptr);

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
