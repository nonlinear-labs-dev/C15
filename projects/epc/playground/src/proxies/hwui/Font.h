#pragma once

#include <playground.h>
#include <freetype2/ft2build.h>
#include <freetype2/freetype/freetype.h>

class Font
{
 public:
  Font(const Glib::ustring &path, int height);
  virtual ~Font();

  typedef int32_t tCoordinate;

  enum class Justification
  {
    Left,
    Center,
    Right
  };

  [[nodiscard]] uint32_t getStringWidth(const Glib::ustring &str) const;
  uint32_t draw(const Glib::ustring &text, tCoordinate x, tCoordinate y);
  [[nodiscard]] int getHeight() const;

 private:
  FT_Face m_face;
  int m_height;

  tCoordinate drawLetter(FT_GlyphSlot slot, tCoordinate x, tCoordinate y);
};
