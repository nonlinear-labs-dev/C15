#pragma once

#include "Setting.h"
#include <vector>

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

  size_t getEnumIndex() const
  {
    return static_cast<size_t>(EnumSetting<tEnum>::get());
  }

  void load(const Glib::ustring &text, Initiator initiator) override
  {
    int i = 0;
    for(const auto &it : enumToString())
    {
      if(text == it)
      {
        set((tEnum) i);
        return;
      }
      i++;
    }
  }

  void incDec(int i, bool wrap)
  {
    inc(i, wrap);
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

  const std::vector<Glib::ustring> &getDisplayStrings() const
  {
    return enumToDisplayString();
  }

  Glib::ustring getDisplayString() const override
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

  EnumSetting(const EnumSetting &other) = delete;
  EnumSetting &operator=(const EnumSetting &) = delete;

 private:
  tEnum m_mode;
  std::weak_ptr<tEnum> m_overlay;
};
