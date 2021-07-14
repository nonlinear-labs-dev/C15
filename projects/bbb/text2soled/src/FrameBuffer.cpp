#include <fcntl.h>
#include "Point.h"
#include "FrameBuffer.h"
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <iostream>

FrameBuffer::FrameBuffer()
{
  openAndMap();
  setColor(FrameBuffer::C255);
}

void FrameBuffer::openAndMap()
{
#ifndef _DEVELOPMENT_PC
  m_fd = open("/dev/fb0", O_RDWR);

  if(m_fd < 0)
    g_error("Could not open framebuffer device file");

  if(ioctl(m_fd, FBIOGET_VSCREENINFO, &m_varInfo) == -1)
    g_error("Could not get fb_var_screeninfo!");

  auto bytesPerPixel = m_varInfo.bits_per_pixel / 8;
  m_buffersize = m_varInfo.xres * m_varInfo.yres * bytesPerPixel;
  m_frontBuffer = reinterpret_cast<tPixel*>(mmap(0, m_buffersize, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0));

  if(m_frontBuffer == MAP_FAILED)
    g_error("Could not memory map buffer");
#endif
}

FrameBuffer::~FrameBuffer()
{
  unmapAndClose();
}

void FrameBuffer::setPixel(tCoordinate x, tCoordinate y)
{
  setOffsetPixel(x, y);
}

inline void FrameBuffer::setOffsetPixel(tCoordinate x, tCoordinate y)
{
  setRawPixel(x, y);
}

inline void FrameBuffer::setRawPixel(tCoordinate x, tCoordinate y)
{
#ifndef _DEVELOPMENT_PC
  const long index = getIndex(x, y);

  if(index >= 0 && index < m_buffersize)
    m_frontBuffer[index] = m_currentColor;
#endif
}

inline long FrameBuffer::getIndex(tCoordinate x, tCoordinate y) const
{
  return (x + y * m_varInfo.xres);
}

void FrameBuffer::unmapAndClose()
{
#ifndef _DEVELOPMENT_PC
  if(munmap(m_frontBuffer, m_buffersize))
    g_error("Error while un-mapping buffer");

  if(close(m_fd))
    g_error("Error while closing file handle: ");
#endif
}

void FrameBuffer::clear()
{
#ifndef _DEVELOPMENT_PC
  memset(m_frontBuffer, 0, m_buffersize);
#endif
}

void FrameBuffer::setColor(const Colors& c)
{
  m_currentColor = c;
}

void FrameBuffer::fiddleColor(tPixel p)
{
  m_currentColor = (Colors)(p);
}

FrameBuffer::Colors FrameBuffer::getColor() const
{
  return m_currentColor;
}

void FrameBuffer::fillRect(tCoordinate left, tCoordinate top, tCoordinate width, tCoordinate height)
{
  fillRect(Rect(left, top, width, height));
}

void FrameBuffer::fillRect(const Rect& rect)
{
  for(auto y = rect.getTop(); y <= rect.getBottom(); y++)
    drawRawHorizontalLine(rect.getLeft(), y, rect.getWidth());
}

inline void FrameBuffer::drawRawHorizontalLine(tCoordinate x, tCoordinate y, tCoordinate length)
{
#ifndef _DEVELOPMENT_PC
  auto fromIdx = getIndex(x, y);

  auto data = m_frontBuffer + fromIdx;

  for(long i = 0; i < length; i++)
    data[i] = m_currentColor;
#endif
}

void FrameBuffer::drawRect(tCoordinate x, tCoordinate y, tCoordinate width, tCoordinate height)
{
  drawRect(Rect(x, y, width, height));
}

void FrameBuffer::drawRect(const Rect& rect)
{
  if(!rect.isEmpty())
  {
    drawHorizontalLine(rect.getLeft(), rect.getTop(), rect.getWidth());
    drawHorizontalLine(rect.getLeft(), rect.getBottom(), rect.getWidth());
    drawVerticalLine(rect.getLeft(), rect.getTop(), rect.getHeight());
    drawVerticalLine(rect.getRight(), rect.getTop(), rect.getHeight());
  }
}

void FrameBuffer::drawHorizontalLine(tCoordinate x, tCoordinate y, tCoordinate length)
{
  drawRawHorizontalLine(x, y, length);
}

void FrameBuffer::drawVerticalLine(tCoordinate x, tCoordinate y, tCoordinate length)
{
  for(auto i = y; i < y + length; i++)
    setRawPixel(x, i);
}
