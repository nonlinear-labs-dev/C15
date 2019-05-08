#pragma once

#include "playground.h"
#include <proxies/hwui/HWUIEnums.h>

class OLEDProxy;
class Font;

class Oleds
{
 public:
  typedef shared_ptr<Font> tFont;

  virtual ~Oleds();

  static Oleds &get();

  void registerProxy(OLEDProxy *proxy);
  void syncRedraw();

  tFont getFont(const Glib::ustring &name, int height);
  void deInit();

 private:
  Oleds();
  Oleds(const Oleds &other);
  Oleds &operator=(const Oleds &);

  bool regularRedraw();

  typedef pair<Glib::ustring, int> tKey;

  list<OLEDProxy *> m_proxies;
  map<tKey, tFont> m_fonts;
};
