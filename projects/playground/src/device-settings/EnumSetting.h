#pragma once

#include "Setting.h"

template <typename TEnum> class EnumSetting : public Setting
{
 private:
  typedef Setting super;

 public:
  typedef TEnum tEnum;

  EnumSetting(UpdateDocumentContributor &settings, tEnum def)
      : super(settings)
      , m_mode(def)
  {
  }

  virtual bool set(tEnum m)
  {
    if(m_mode != m)
    {
      m_mode = m;
      notify();
      return true;
    }
    return false;
  }

  tEnum get() const
  {
    if(auto overlay = m_overlay.lock())
      return *overlay;

    return m_mode;
  }

  void load(const Glib::ustring &text)
  {
    auto toLower = [](const std::string &s) {
      std::string ret;
      for(auto &c : s)
      {
        ret.push_back(std::tolower(c));
      }
      return ret;
    };

    auto newText = toLower(text);

    int i = 0;
    for(const auto &it : enumToString())
    {
      if(newText == toLower(it))
      {
        set((tEnum) i);
        return;
      }
      i++;
    }

    g_warn_if_reached();
  }

  void inc(int dir, bool wrap)
  {
    int numEntries = enumToString().size();
    int e = (int) m_mode;

    while(dir > 0)
    {
      dir--;
      e++;

      if(e >= numEntries)
      {
        if(wrap)
          e = 0;
        else
          e = numEntries - 1;

        break;
      }
    }

    while(dir < 0)
    {
      dir++;
      e--;
      if(e < 0)
      {
        if(wrap)
          e = numEntries - 1;
        else
          e = 0;

        break;
      }
    }

    set((tEnum) e);
  }

  Glib::ustring save() const
  {
    int idx = static_cast<int>(get());
    return enumToString()[idx];
  }

  Glib::ustring getDisplayString() const
  {
    int idx = static_cast<int>(get());
    return enumToDisplayString()[idx];
  }

  virtual const std::vector<Glib::ustring> &enumToString() const = 0;
  virtual const std::vector<Glib::ustring> &enumToDisplayString() const = 0;

  std::shared_ptr<tEnum> scopedOverlay(tEnum value)
  {
    auto ret = std::make_shared<tEnum>(value);
    m_overlay = ret;
    return ret;
  }

 private:
  EnumSetting(const EnumSetting &other);
  EnumSetting &operator=(const EnumSetting &);

  tEnum m_mode;
  std::weak_ptr<tEnum> m_overlay;
};
