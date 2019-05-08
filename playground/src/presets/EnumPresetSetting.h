#pragma once

#include "PresetSetting.h"

class Settings;

template <typename tEnum> class EnumPresetSetting : public PresetSetting
{
 private:
  typedef PresetSetting super;

 public:
  EnumPresetSetting(PresetSettings &settings, tEnum def)
      : super(settings)
      , m_mode(def)
  {
  }

  virtual void set(UNDO::Scope::tTransactionPtr transaction, tEnum m)
  {
    if(m_mode != m)
    {
      auto swapData = UNDO::createSwapData(m);

      transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
        swapData->swapWith(m_mode);
        onChange(false);
      });
    }
  }

  tEnum get() const
  {
    return m_mode;
  }

  void load(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &text)
  {
    int i = 0;
    for(const auto &it : enumToString())
    {
      if(text == it)
      {
        set(transaction, (tEnum) i);
        return;
      }
      i++;
    }
  }

  void inc(UNDO::Scope::tTransactionPtr transaction)
  {
    int e = (int) m_mode;
    e++;

    if(e >= (int) enumToString().size())
      e = 0;

    set(transaction, (tEnum) e);
  }

  Glib::ustring save() const
  {
    int idx = static_cast<int>(get());
    return enumToString()[idx];
  }

 protected:
  virtual const vector<Glib::ustring> &enumToString() const = 0;

 private:
  EnumPresetSetting(const EnumPresetSetting &other);
  EnumPresetSetting &operator=(const EnumPresetSetting &);

  tEnum m_mode;
};
