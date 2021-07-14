#include "Font.h"
#include "FrameBuffer.h"

namespace
{
  static FT_Library s_library = nullptr;

  template <typename... Args> bool check(FT_Error err, Args... args)
  {
    if(err)
      g_error("problem with the font");

    return err == 0;
  }
}

Font::Font(const Glib::ustring &path, int height)
    : m_height(height)
{
  if(!s_library)
    check(FT_Init_FreeType(&s_library), __LINE__);

  check(FT_New_Face(s_library, path.c_str(), 0, &m_face), __LINE__, path);
  check(FT_Set_Pixel_Sizes(m_face, 0, height), __LINE__, height);
}

Font::~Font()
{
  FT_Done_Face(m_face);
}

uint32_t Font::getStringWidth(const Glib::ustring &text) const
{
  uint32_t x = 0;

  for(auto c : text)
    if(check(FT_Load_Char(m_face, c, FT_LOAD_NO_BITMAP), __LINE__, c))
      x += m_face->glyph->advance.x;

  return x >> 6;
}

uint32_t Font::draw(const Glib::ustring &text, tCoordinate x, tCoordinate y)
{
  auto oldX = x;
  x <<= 6;

  for(auto c : text)
    if(check(FT_Load_Char(m_face, c, FT_LOAD_RENDER), __LINE__, c))
      x += drawLetter(m_face->glyph, x, y);

  x >>= 6;
  return x - oldX;
}

inline bool isPixelSet(FT_GlyphSlot slot, int srcX, int srcY)
{
  auto index = srcX + srcY * slot->bitmap.width;
  return slot->bitmap.buffer[index];
}

Font::tCoordinate Font::drawLetter(FT_GlyphSlot slot, tCoordinate x, tCoordinate y)
{
  x >>= 6;

  for(int srcX = 0; srcX < slot->bitmap.width; srcX++)
    for(int srcY = 0; srcY < slot->bitmap.rows; srcY++)
      if(isPixelSet(slot, srcX, srcY))
        FrameBuffer::get().setPixel(srcX + x + slot->bitmap_left, (m_height - slot->bitmap_top) + srcY + y - m_height);

  return slot->advance.x;
}

int Font::getHeight() const
{
  return m_height;
}
