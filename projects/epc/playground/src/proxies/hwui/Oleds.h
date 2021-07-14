#pragma once

#include "playground.h"
#include <proxies/hwui/HWUIEnums.h>
#include <nltools/threading/Throttler.h>
#include <map>

class OLEDProxy;
class Font;

class Oleds
{
 public:
  typedef std::shared_ptr<Font> tFont;

  virtual ~Oleds();

  static Oleds &get();

  void registerProxy(OLEDProxy *proxy);
  void syncRedraw();
  void setDirty();
  [[nodiscard]] bool isDirty() const;

  tFont getFont(const Glib::ustring &name, int height);
  void deInit();

 private:
  Oleds();
  Oleds(const Oleds &other);
  Oleds &operator=(const Oleds &);

  typedef std::pair<Glib::ustring, int> tKey;

  std::list<OLEDProxy *> m_proxies;
  std::map<tKey, tFont> m_fonts;
  Throttler m_throttler;
};
