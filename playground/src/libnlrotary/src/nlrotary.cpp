#include <glib.h>
#include <gio/gio.h>
#include <stdlib.h>
#include <memory.h>
#include "nlrotary.h"

#define READ_BUFFER_SIZE 128

static const char *PATH_TAPER_1 = "/sys/devices/platform/omap/tsc/ain1";
static const char *PATH_TAPER_2 = "/sys/devices/platform/omap/tsc/ain2";

static rotary_callback_t s_the_callback = NULL;

typedef struct _AINFile
{
  const char *path;
  char read_buffer[READ_BUFFER_SIZE];
  int current_value;
  int update_number;
} AINFile;

AINFile s_theFiles[2];

static void nl_rotary_init();
static gboolean start_reading_files(gpointer useless);
static void start_reading(AINFile *file);
static void nl_async_open_stream(GObject *source_object, GAsyncResult *res, gpointer user_data);
static void nl_async_read(GObject *source_object, GAsyncResult *res, gpointer user_data);
static void on_input_updated();
static void on_inputs_updated();

///////////////////////////////////////////////
// public API

void nl_rotary_register_callback(rotary_callback_t cb)
{
  g_assert(s_the_callback == NULL);
  s_the_callback = cb;

  nl_rotary_init();
}

rotary_movement_t nl_rotary_query_resolution(rotary_id_t id)
{
  return 1;
}

int nl_rotary_query_number_of_rotaries()
{
  return 1;
}

///////////////////////////////////////////////
// private API

static void nl_rotary_init()
{
  system("modprobe ti_tscadc");

  memset(&s_theFiles[0], 0, sizeof(AINFile));
  memset(&s_theFiles[1], 0, sizeof(AINFile));
  s_theFiles[0].path = PATH_TAPER_1;
  s_theFiles[1].path = PATH_TAPER_2;

  start_reading_files(NULL);
}

static gboolean start_reading_files(gpointer useless)
{
  start_reading(&s_theFiles[0]);
  start_reading(&s_theFiles[1]);
  return FALSE;
}

static void start_reading(AINFile *file)
{
  GFile *in = g_file_new_for_path(file->path);
  g_file_read_async(in, G_PRIORITY_LOW, NULL, nl_async_open_stream, file);
  g_object_unref(in);
}

static void nl_async_open_stream(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
  AINFile *file = (AINFile *) user_data;
  GError *error = NULL;
  GFileInputStream *stream = g_file_read_finish((GFile *) source_object, res, &error);

  if(error)
    g_error("error opening the file handle: %s\n", error->message);

  g_input_stream_read_async((GInputStream *) stream, file->read_buffer, READ_BUFFER_SIZE, G_PRIORITY_LOW, NULL,
                            nl_async_read, file);
}

static void nl_async_read(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
  AINFile *file = (AINFile *) user_data;
  GInputStream *stream = (GInputStream *) source_object;
  GError *error = NULL;
  g_input_stream_close_finish(stream, res, &error);

  if(error)
    g_error("error reading from stream: %s\n", error->message);

  file->current_value = atoi(file->read_buffer);
  file->update_number++;

  g_object_unref(stream);

  on_input_updated();
}

static void on_input_updated()
{
  if(s_theFiles[0].update_number == s_theFiles[1].update_number)
  {
    on_inputs_updated();
  }
}

static void on_inputs_updated()
{
  g_print("ain1: %d, ain2: %d\n", s_theFiles[0].current_value, s_theFiles[1].current_value);
  s_the_callback(0, s_theFiles[0].current_value);

  g_timeout_add(20, start_reading_files, NULL);
}

///////////////////////////////////////////////
// Test code

void rotary_callback(rotary_id_t knobID, rotary_movement_t amount)
{
  g_print("Rotary %d turned by %d\n", knobID, amount);
}

// main entry point just for testing
void nl_rotary_test()
{
  nl_rotary_register_callback(&rotary_callback);
}
