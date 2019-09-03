#include <Application.h>
#include <bbbb.h>
#include <fcntl.h>
#include <io/framebuffer/FrameBufferSender.h>
#include <nltools/logging/Log.h>
#include <Options.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>

FrameBufferSender::FrameBufferSender()
    : m_throttler(std::chrono::milliseconds(10))
{
#ifndef _DEVELOPMENT_PC
  m_fd = open("/dev/fb0", O_RDWR);

  if(m_fd < 0)
  {
    nltools::Log::warning("Could not open framebuffer device file");
    return;
  }

  if(ioctl(m_fd, FBIOGET_VSCREENINFO, &m_varInfo) == -1)
  {
    nltools::Log::warning("Could not get fb_var_screeninfo!");
    return;
  }

  auto bytesPerPixel = m_varInfo.bits_per_pixel / 8;
  m_buffersize = m_varInfo.xres * m_varInfo.yres * bytesPerPixel;
  m_frontBuffer = reinterpret_cast<tPixel*>(mmap(0, m_buffersize, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0));

  if(m_frontBuffer == MAP_FAILED)
  {
    nltools::Log::warning("Could not memory map buffer");
    return;
  }
#endif
}

FrameBufferSender::~FrameBufferSender()
{
}

void FrameBufferSender::send(tMessage msg)
{
#ifndef _DEVELOPMENT_PC
  m_throttler.doTask([=]() {
    gsize numBytes = 0;
    auto bytes = reinterpret_cast<const int8_t*>(msg->get_data(numBytes));
    memcpy(m_frontBuffer, bytes, numBytes);
    msync(m_frontBuffer, numBytes, MS_ASYNC);
  });
#endif
}

void FrameBufferSender::send(const void* data, size_t numBytes)
{
  memcpy(m_backBuffer, data, numBytes);

#ifndef _DEVELOPMENT_PC
  m_throttler.doTask([=]() {
    memcpy(m_frontBuffer, m_backBuffer, numBytes);
    msync(m_frontBuffer, numBytes, MS_ASYNC);
  });
#endif
}
