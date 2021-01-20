#pragma once

#include "Setting.h"
#include <nltools/enums/EnumTools.h>
#include "xml/Writer.h"
#include "xml/Attribute.h"
#include <nltools/Types.h>

template <typename TEnum> class NLEnumSetting : public Setting
{
 private:
  typedef Setting super;

 public:
  typedef TEnum tEnum;

  NLEnumSetting(UpdateDocumentContributor &settings, tEnum def)
      : super(settings)
      , m_mode(def)
      , m_displayStrings { [] {
        auto in = getAllStrings<tEnum>();
        std::vector<Glib::ustring> ret {};
        ret.reserve(in.size());
        for(auto &i : in)
        {
          ret.emplace_back(i);
        }
        return ret;
      }() }
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

  bool forceset(tEnum m)
  {
    m_mode = m;
    notify();
    return true;
  }

  tEnum get() const
  {
    return m_mode;
  }

  void load(const Glib::ustring &text, Initiator) override
  {
    set(to<TEnum>(text));
  }

  void incDec(int dir, bool wrap)
  {
    auto v = getAllValues<tEnum>();
    auto it = std::find(v.begin(), v.end(), get());
    auto idx = std::distance(v.begin(), it);
    idx += dir;

    if(idx < 0 && wrap)
      idx = v.size() + idx;

    if(idx >= v.size() && wrap)
      idx = idx - v.size();

    if(idx >= 0 && idx < v.size())
      set(v.at(idx));
  }

  Glib::ustring save() const
  {
    return toString(get());
  }

  void writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const override
  {
    bool changed = knownRevision < getUpdateIDOfLastChange();
    writer.writeTextElement("value", toString(get()), Attribute("changed", changed));
  }

  Glib::ustring getDisplayString() const override
  {
    auto v = getAllValues<TEnum>();
    auto it = std::find(v.begin(), v.end(), get());
    if(it != v.end())
    {
      auto idx = std::distance(v.begin(), it);
      return getAllStrings<TEnum>().at(idx);
    }

    return "";
  }

  virtual const std::vector<Glib::ustring> &getDisplayStrings() const
  {
    return m_displayStrings;
  }

 private:
  std::vector<Glib::ustring> m_displayStrings;
  NLEnumSetting(const NLEnumSetting &other);
  NLEnumSetting &operator=(const NLEnumSetting &);

  tEnum m_mode;
};
