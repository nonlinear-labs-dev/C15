#include <libundo/undo/Command.h>
#include <libundo/undo/SwapData.h>
#include <playground.h>
#include <presets/AttributesOwner.h>
#include <testing/TestDriver.h>
#include <xml/Attribute.h>
#include <utility>

static TestDriver<AttributesOwner> attributesOwnerTests;

AttributesOwner::AttributesOwner()
{
}

AttributesOwner::~AttributesOwner()
{
}

const AttributesOwner::tAttributes &AttributesOwner::getAttributes() const
{
  return m_attributes;
}

void AttributesOwner::setAttribute(const Glib::ustring &key, const Glib::ustring &value)
{
  m_attributes[key] = value;
  onChange();
}

void AttributesOwner::undoableSetAttribute(UNDO::Scope::tTransactionPtr transaction, const Glib::ustring &key,
                                           const Glib::ustring &value)
{
  auto swapData = UNDO::createSwapData(value);

  transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
    Glib::ustring curValue = getAttribute(key, "");
    swapData->swapWith(curValue);

    if(curValue.empty())
      m_attributes.erase(key);
    else
      m_attributes[key] = curValue;

    onChange();
  });
}

void AttributesOwner::undoableClearAttributes(UNDO::Scope::tTransactionPtr transaction)
{
  auto swapData = UNDO::createSwapData(tAttributes());

  transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
    swapData->swapWith(m_attributes);
    onChange();
  });
}

Glib::ustring AttributesOwner::getAttribute(const Glib::ustring &key, const Glib::ustring &def) const
{
  auto it = m_attributes.find(key);

  if(it != m_attributes.end())
    return it->second;

  return def;
}

void AttributesOwner::copyFrom(UNDO::Scope::tTransactionPtr transaction, AttributesOwner *other)
{
  auto swapData = UNDO::createSwapData(other->m_attributes);

  transaction->addSimpleCommand([=](UNDO::Command::State) mutable {
    swapData->swapWith<0>(m_attributes);
    onChange();
  });
}

size_t AttributesOwner::getHash() const
{
  size_t hash = 0;

  for(auto &a : m_attributes)
  {
    hash_combine(hash, a.first);
    hash_combine(hash, a.second);
  }

  return hash;
}

bool AttributesOwner::doesAnyAttributeMatch(const Glib::ustring &part) const
{
  for(auto &a : m_attributes)
  {
    if(doesAttributeMatch(a.second, part))
    {
      return true;
    }
  }
  return false;
}

bool AttributesOwner::doesAttributeMatch(const Glib::ustring &attribute, const Glib::ustring &query)
{
  auto lower = attribute.lowercase();
  auto pos = lower.find(query);

  if(pos != Glib::ustring::npos)
  {
    if(isHashTag(query))
    {
      return true;
    }

    if(!isHashTag(lower, pos))
    {
      return true;
    }
  }

  return false;
}

bool AttributesOwner::isHashTag(const Glib::ustring &str, size_t pos)
{
  auto whiteSpacePos = str.rfind(' ', pos);
  auto hashTagPosition = 0;

  if(whiteSpacePos == Glib::ustring::npos)
  {
    return str[0] == '#';
  }
  else if(whiteSpacePos == pos)
  {
    return false;
  }
  else
  {
    return str[whiteSpacePos + 1] == '#';
  }
}

void AttributesOwner::writeAttributes(Writer &writer) const
{
  for(auto &it : getAttributes())
    writer.writeTextElement("attribute", it.second, Attribute("key", it.first));
}

void AttributesOwner::registerTests()
{
  g_test_add_func("/AttributesOwner/isHashTag", []() {
    g_assert_true(AttributesOwner::isHashTag("#abc"));
    g_assert_true(AttributesOwner::isHashTag("#a"));
    g_assert_true(AttributesOwner::isHashTag("#"));
    g_assert_true(AttributesOwner::isHashTag(" #abc"));
    g_assert_true(AttributesOwner::isHashTag(" #a"));

    g_assert_false(AttributesOwner::isHashTag("def #abc ghi", 0));
    g_assert_false(AttributesOwner::isHashTag("def #abc ghi", 1));
    g_assert_false(AttributesOwner::isHashTag("def #abc ghi", 2));
    g_assert_false(AttributesOwner::isHashTag("def #abc ghi", 3));

    g_assert_true(AttributesOwner::isHashTag("def #abc ghi", 4));
    g_assert_true(AttributesOwner::isHashTag("def #abc ghi", 5));
    g_assert_true(AttributesOwner::isHashTag("def #abc ghi", 6));
    g_assert_true(AttributesOwner::isHashTag("def #abc ghi", 7));

    g_assert_false(AttributesOwner::isHashTag("def #abc ghi", 8));
    g_assert_false(AttributesOwner::isHashTag("def #abc ghi", 9));
    g_assert_false(AttributesOwner::isHashTag("def #abc ghi", 10));
    g_assert_false(AttributesOwner::isHashTag("def #abc ghi", 11));

    g_assert_false(AttributesOwner::isHashTag("def #abc ghi", 100));
    g_assert_false(AttributesOwner::isHashTag(" abc"));
    g_assert_false(AttributesOwner::isHashTag(""));
    g_assert_false(AttributesOwner::isHashTag(" # abc"));

    g_assert_true(AttributesOwner::isHashTag(" ags uyr j #abc", 11));
    g_assert_true(AttributesOwner::isHashTag(" ags uyr j #a", 11));
    g_assert_true(AttributesOwner::isHashTag(" ags uyr j #", 11));

    g_assert_false(AttributesOwner::isHashTag(" ags uyr j  #a ", 11));
    g_assert_false(AttributesOwner::isHashTag(" ags uyr j  #abc", 11));
    g_assert_false(AttributesOwner::isHashTag(" ags uyr j  abc", 11));
    g_assert_false(AttributesOwner::isHashTag(" ags uyr j ", 11));
    g_assert_false(AttributesOwner::isHashTag(" ags uyr j  # abc", 11));
  });

  g_test_add_func("/AttributesOwner/doesAttributeMatch", []() {
    g_assert_false(AttributesOwner::doesAttributeMatch("eins zwei drei", "#eins"));
    g_assert_false(AttributesOwner::doesAttributeMatch("eins zwei drei", "#zwei"));
    g_assert_false(AttributesOwner::doesAttributeMatch("eins zwei drei", "#drei"));

    g_assert_true(AttributesOwner::doesAttributeMatch("eins zwei drei", "eins"));
    g_assert_true(AttributesOwner::doesAttributeMatch("eins zwei drei", "zwei"));
    g_assert_true(AttributesOwner::doesAttributeMatch("eins zwei drei", "drei"));

    g_assert_true(AttributesOwner::doesAttributeMatch("#eins zwei drei", "#eins"));
    g_assert_false(AttributesOwner::doesAttributeMatch("#eins zwei drei", "#zwei"));
    g_assert_false(AttributesOwner::doesAttributeMatch("#eins zwei drei", "#drei"));
    g_assert_true(AttributesOwner::doesAttributeMatch("#eins zwei drei", "zwei"));
    g_assert_true(AttributesOwner::doesAttributeMatch("#eins zwei drei", "drei"));

    g_assert_false(AttributesOwner::doesAttributeMatch("eins #zwei drei", "#eins"));
    g_assert_true(AttributesOwner::doesAttributeMatch("eins #zwei drei", "#zwei"));
    g_assert_false(AttributesOwner::doesAttributeMatch("eins #zwei drei", "#drei"));

    g_assert_false(AttributesOwner::doesAttributeMatch("eins zwei #drei", "#eins"));
    g_assert_false(AttributesOwner::doesAttributeMatch("eins zwei #drei", "#zwei"));
    g_assert_true(AttributesOwner::doesAttributeMatch("eins zwei #drei", "#drei"));
  });
}
