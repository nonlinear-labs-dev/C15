#pragma once

#include "Setting.h"
#include "Settings.h"

template<typename TEnum>
  class EnumSetting : public Setting
  {
    private:
      typedef Setting super;

    public:
      typedef TEnum tEnum;

      EnumSetting (Settings &settings, tEnum def) :
          super (settings),
          m_mode (def)
      {
      }

      virtual bool set (tEnum m)
      {
        if (m_mode != m)
        {
          m_mode = m;
          notify ();
          return true;
        }
        return false;
      }

      Settings *getSettings ()
      {
        return static_cast<Settings*> (getParent ());
      }

      tEnum get () const
      {
        if(auto overlay = m_overlay.lock())
          return *overlay;

        return m_mode;
      }

      void load (const Glib::ustring &text)
      {
        int i = 0;
        for (const auto &it : enumToString ())
        {
          if (text == it)
          {
            set ((tEnum) i);
            return;
          }
          i++;
        }
      }

      void inc (int dir = 1)
      {
        int numEntries = enumToString ().size ();
        int e = (int) m_mode;

        while (dir > 0)
        {
          dir--;
          e++;
          if (e >= numEntries)
            e = 0;
        }

        while (dir < 0)
        {
          dir++;
          e--;
          if (e < 0)
            e = numEntries - 1;
        }

        set ((tEnum) e);
      }

      Glib::ustring save () const
      {
        int idx = static_cast<int> (get ());
        return enumToString ()[idx];
      }

      Glib::ustring getDisplayString () const
      {
        int idx = static_cast<int> (get ());
        return enumToDisplayString ()[idx];
      }

      virtual const vector<Glib::ustring> &enumToString () const = 0;
      virtual const vector<Glib::ustring> &enumToDisplayString () const = 0;

      std::shared_ptr<tEnum> scopedOverlay(tEnum value)
      {
        auto ret = std::make_shared<tEnum>(value);
        m_overlay = ret;
        return ret;
      }

    private:
      EnumSetting (const EnumSetting& other);
      EnumSetting& operator= (const EnumSetting&);

      tEnum m_mode;
      std::weak_ptr<tEnum> m_overlay;
  };

