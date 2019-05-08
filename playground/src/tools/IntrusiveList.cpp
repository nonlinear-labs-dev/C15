#include "IntrusiveList.h"
#include "testing/TestDriver.h"

class Item : public IntrusiveListItem<Item *>
{
 public:
  Item(int i)
      : m_i(i)
  {
  }

  int m_i;
};

class List : public IntrusiveList<Item *>
{
 public:
  static void registerTests()
  {
    g_test_add_func("/IntrusiveList/1", []() {
      List l;
      l.append(new Item(1));
      l.append(new Item(2));
      l.append(new Item(3));

      int expected = 1;

      for(Item *i : l)
      {
        g_assert(i->m_i == expected);
        expected++;
      }

      g_assert(expected == 4);

      l.prepend(new Item(0));

      expected = 0;

      for(Item *i : l)
      {
        g_assert(i->m_i == expected);
        expected++;
      }

      g_assert(expected == 4);
    });

    g_test_add_func("/IntrusiveList/removeMiddle", []() {
      List l;
      auto one = new Item(1);

      l.append(new Item(2));
      l.append(new Item(3));
      l.prepend(one);
      l.prepend(new Item(0));

      int expected = 0;

      for(Item *i : l)
      {
        g_assert(i->m_i == expected);
        expected++;
      }

      g_assert(expected == 4);

      l.remove(one);

      expected = 0;

      for(Item *i : l)
      {
        g_assert(i->m_i == expected);
        expected++;

        if(expected == 1)
          expected = 2;
      }

      g_assert(expected == 4);
    });

    g_test_add_func("/IntrusiveList/removeFirst", []() {
      List l;
      auto first = new Item(0);

      l.append(new Item(2));
      l.append(new Item(3));
      l.prepend(new Item(1));
      l.prepend(first);
      l.remove(first);

      int expected = 1;

      for(Item *i : l)
      {
        g_assert(i->m_i == expected);
        expected++;
      }

      g_assert(expected == 4);
    });

    g_test_add_func("/IntrusiveList/removeLast", []() {
      List l;
      auto last = new Item(3);

      l.append(new Item(2));
      l.append(last);
      l.prepend(new Item(1));
      l.prepend(new Item(0));
      l.remove(last);

      int expected = 0;

      for(Item *i : l)
      {
        g_assert(i->m_i == expected);
        expected++;
      }

      g_assert(expected == 3);
    });

    g_test_add_func("/IntrusiveList/clear", []() {
      List l;
      auto last = new Item(3);

      l.append(new Item(2));
      l.append(last);
      l.prepend(new Item(1));
      l.prepend(new Item(0));
      l.remove(last);

      int expected = 0;

      for(Item *i : l)
      {
        g_assert(i->m_i == expected);
        expected++;
      }

      g_assert(expected == 3);
      l.clear();
    });
  }
};

static TestDriver<List> testDriver;
