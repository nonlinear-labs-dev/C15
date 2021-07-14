#pragma once

#include <nltools/Uncopyable.h>

template <typename TItemPtr> class IntrusiveList;
template <typename TSelfPtr> class IntrusiveListItem;

template <typename TSelfPtr> class IntrusiveListItem : public Uncopyable
{
 public:
  using tSelfPtr = TSelfPtr;

  ~IntrusiveListItem() = default;

  tSelfPtr getNext() const
  {
    return next;
  }

  tSelfPtr getPrev() const
  {
    return prev;
  }

 private:
  tSelfPtr next = { nullptr };
  tSelfPtr prev = { nullptr };
  friend class IntrusiveList<TSelfPtr>;
};

template <typename TItemPtr> class IntrusiveList
{
 public:
  using tItemPtr = TItemPtr;
  using tItem = IntrusiveListItem<tItemPtr>;

  IntrusiveList() = default;
  virtual ~IntrusiveList() = default;

  void prepend(tItemPtr item)
  {
    if(first)
      first->tItem::prev = item;

    item->tItem::next = first;
    first = item;

    if(last == nullptr)
      last = item;
  }

  void append(tItemPtr item)
  {
    if(last)
      last->tItem::next = item;

    item->tItem::prev = last;
    last = item;

    if(first == nullptr)
      first = item;
  }

  void insert(tItemPtr beforeThis, tItemPtr item)
  {
    if(beforeThis == nullptr)
    {
      append(item);
    }
    else if(beforeThis == first)
    {
      prepend(item);
    }
    else
    {
      item->tItem::next = beforeThis;
      item->tItem::prev = beforeThis->tItem::prev;
      item->tItem::prev->tItem::next = item;
      beforeThis->tItem::prev = item;
    }
  }

  void remove(tItemPtr item)
  {
    if(item->tItem::next)
      item->tItem::next->tItem::prev = item->tItem::prev;

    if(item->tItem::prev)
      item->tItem::prev->tItem::next = item->tItem::next;

    if(item == first)
      first = item->tItem::next;

    if(item == last)
      last = item->tItem::prev;

    item->tItem::next = tItemPtr(nullptr);
    item->tItem::prev = tItemPtr(nullptr);
  }

  void clear()
  {
    while(first)
      remove(first);
  }

  template <typename Callback> void forEach(Callback cb)
  {
    tItemPtr walker = first;

    while(walker)
    {
      tItemPtr next = walker->tItem::next;
      cb(walker);
      walker = next;
    }
  }

  void deleteItems()
  {
    forEach([](tItemPtr p) { delete p; });
  }

  class Iterator
  {
   public:
    explicit Iterator(tItemPtr item)
        : m_item(item)
    {
    }

    Iterator &operator++()
    {
      m_item = m_item->tItem::getNext();
      return *this;
    }

    Iterator &operator++(int i)
    {
      m_item = m_item->tItem::getNext();
      return *this;
    }

    tItemPtr &operator*()
    {
      return m_item;
    }

    tItemPtr &operator->()
    {
      return m_item;
    }

    bool operator!=(const Iterator &other) const
    {
      return other.m_item != m_item;
    }

    bool operator==(const Iterator &other) const
    {
      return other.m_item == m_item;
    }

   private:
    tItemPtr m_item;
  };

  class ConstIterator
  {
   public:
    explicit ConstIterator(const tItemPtr item)
        : m_item(item)
    {
    }

    ConstIterator &operator++()
    {
      m_item = m_item->tItem::getNext();
      return *this;
    }

    ConstIterator &operator++(int i)
    {
      m_item = m_item->tItem::getNext();
      return *this;
    }

    const tItemPtr &operator*()
    {
      return m_item;
    }

    const tItemPtr &operator->()
    {
      return m_item;
    }

    bool operator!=(const ConstIterator &other) const
    {
      return other.m_item != m_item;
    }

    bool operator==(const ConstIterator &other) const
    {
      return other.m_item == m_item;
    }

   private:
    tItemPtr m_item;
  };

  Iterator begin()
  {
    return Iterator(first);
  }

  Iterator end()
  {
    return Iterator(nullptr);
  }

  ConstIterator begin() const
  {
    return ConstIterator(first);
  }

  ConstIterator end() const
  {
    return ConstIterator(nullptr);
  }

 private:
  tItemPtr first = { nullptr };
  tItemPtr last = { nullptr };
};
