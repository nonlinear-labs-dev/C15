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
      , m_displayStrings { getMap<tEnum>().size() }
  {
    auto in = getAllStrings<tEnum>();
    std::transform(in.begin(), in.end(), m_displayStrings.begin(), [](auto r) { return r; });
    assert(in.size() == m_displayStrings.size());
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

  virtual void incDec(int dir, bool wrap)
  {
    const auto &v = m_displayStrings;
    auto it = std::find(v.begin(), v.end(), getDisplayString());
    auto idx = std::distance(v.begin(), it);
    idx += dir;

    if(idx < 0 && wrap)
      idx = v.size() + idx;

    if(idx >= v.size() && wrap)
      idx = idx - v.size();

    if(idx >= 0 && idx < v.size())
      set((tEnum) idx);
  }

  Glib::ustring save() const override
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

  NLEnumSetting(const NLEnumSetting &other) = delete;
  NLEnumSetting &operator=(const NLEnumSetting &) = delete;

 private:
  std::vector<Glib::ustring> m_displayStrings;

  tEnum m_mode;
};
