#include <Application.h>
#include <device-settings/DebugLevel.h>
#include <device-settings/Settings.h>
#include <errno.h>
#include <fcntl.h>
#include <gio/gunixoutputstream.h>
#include <glib/gstdio.h>
#include <Options.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <proxies/hwui/panel-unit/boled/SplashLayout.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <sys/stat.h>
#include <unistd.h>
#include <xml/FileOutStream.h>
#include <tools/FileTools.h>

FileOutStream::FileOutStream(const Glib::ustring &fileName, bool zip)
    : m_filename(fileName)
{
  m_kioskMode = getKioskMode();

  if(m_kioskMode)
    return;

  auto tmpFileName = getTmpFileName();
  g_unlink(tmpFileName.c_str());

  m_fileHandle = open(tmpFileName.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

  auto fileStream = g_unix_output_stream_new(m_fileHandle, FALSE);
  auto buffered = g_buffered_output_stream_new(fileStream);

  if(zip)
  {
    auto compressor = g_zlib_compressor_new(GZlibCompressorFormat::G_ZLIB_COMPRESSOR_FORMAT_GZIP, 5);
    auto converter = g_converter_output_stream_new(buffered, G_CONVERTER(compressor));
    m_stream = g_data_output_stream_new(G_OUTPUT_STREAM(converter));
    g_object_unref(converter);
    g_object_unref(compressor);
  }
  else
  {
    m_stream = g_data_output_stream_new(buffered);
  }

  g_object_unref(buffered);
  g_object_unref(fileStream);
}

FileOutStream::~FileOutStream()
{
  if(m_kioskMode)
    return;

  if(m_autoCommit)
    commit();

  g_unlink(getTmpFileName().c_str());
}

Glib::ustring FileOutStream::getTmpFileName()
{
  return m_filename + "_tmp";
}

void FileOutStream::setAutoCommit(bool ac)
{
  m_autoCommit = ac;
}

void FileOutStream::commit()
{
  if(m_kioskMode)
    return;

  if(m_stream && m_fileHandle >= 0)
  {
    GError *error = nullptr;
    g_output_stream_close(G_OUTPUT_STREAM(m_stream), nullptr, &error);

    if(error)
    {
      DebugLevel::error(__PRETTY_FUNCTION__, __LINE__, error->message);
      g_error_free(error);
    }

    g_object_unref(m_stream);

    fsync(m_fileHandle);
    close(m_fileHandle);
    m_fileHandle = -1;

    auto oldName = getTmpFileName();

    if(g_rename(oldName.c_str(), m_filename.c_str()))
    {
      DebugLevel::error("FileOutStream: Could not rename tmp file", oldName, "to target file", m_filename, ":",
                        g_strerror(errno), " trying with backup option! (FileTools::rename)");

      FileTools::rename(oldName, m_filename);
    }
  }
}

void FileOutStream::implWrite(const Glib::ustring &str)
{
  if(m_kioskMode)
    return;

  GError *error = nullptr;
  g_data_output_stream_put_string(m_stream, str.c_str(), nullptr, &error);

  if(error)
  {
    DebugLevel::error(error->message);
    g_error_free(error);
  }
}

void FileOutStream::implWrite(const void *buf, size_t numBytes)
{
  if(m_kioskMode)
    return;

  GError *error = nullptr;
  gsize bytesWritten = 0;

  if(!g_output_stream_write_all(G_OUTPUT_STREAM(m_stream), buf, numBytes, &bytesWritten, nullptr, &error))
  {
    if(error)
    {
      DebugLevel::error(error->message);
      g_error_free(error);
    }
    throw std::runtime_error("writing to file failed");
  }
}

void FileOutStream::setKioskMode(bool kiosk)
{
  if(kiosk)
  {
    auto ret = FileTools::writeToFile(Application::get().getOptions()->getKioskModeFile(), "1");
    DebugLevel::warning("writing kioskMode: ", ret, "path: ", Application::get().getOptions()->getKioskModeFile());
  }
  else
  {
    auto ret = FileTools::writeToFile(Application::get().getOptions()->getKioskModeFile(), "0");
    DebugLevel::warning("writing kioskMode: ", ret, "path: ", Application::get().getOptions()->getKioskModeFile());

    auto &app = Application::get();
    auto pm = app.getPresetManager();
    auto &boled = app.getHWUI()->getPanelUnit().getEditPanel().getBoled();

    pm->undoableClear();
    boled.setOverlay(new SplashLayout());
    app.getSettings()->reload();
    pm->reload();
    boled.resetOverlay();
  }
}

bool FileOutStream::getKioskMode()
{
  return FileTools::readFromFile(Application::get().getOptions()->getKioskModeFile()).find("1") != Glib::ustring::npos
      ? true
      : false;
}
