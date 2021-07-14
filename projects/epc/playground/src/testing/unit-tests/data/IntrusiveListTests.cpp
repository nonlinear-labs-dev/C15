#include <catch.hpp>
#include <testing/TestHelper.h>

template <typename tData> class Item : public IntrusiveListItem<Item<tData> *>
{
 public:
  explicit Item(tData t)
      : data { t }
  {
  }
  tData data;
};

template <typename tData> class List : public IntrusiveList<Item<tData> *>
{
};

TEST_CASE("Simple IntrusiveList Operations")
{
  List<int> l;

  SECTION("Simple append")
  {
    l.append(new Item<int>(1));
    l.append(new Item<int>(2));
    l.append(new Item<int>(3));

    int expected = 1;

    for(auto i : l)
    {
      CHECK(i->data == expected);
      expected++;
    }

    CHECK(expected == 4);

    expected = 0;

    l.prepend(new Item<int>(0));

    for(auto i : l)
    {
      CHECK(i->data == expected);
      expected++;
    }

    CHECK(expected == 4);
  }

  SECTION("Remove Middle")
  {
    auto one = new Item<int>(1);

    l.append(new Item<int>(2));
    l.append(new Item<int>(3));
    l.prepend(one);
    l.prepend(new Item<int>(0));

    int expected = 0;

    for(auto i : l)
    {
      CHECK(i->data == expected);
      expected++;
    }

    CHECK(expected == 4);

    l.remove(one);

    expected = 0;

    for(auto i : l)
    {
      CHECK(i->data == expected);
      expected++;

      if(expected == 1)
        expected = 2;
    }

    CHECK(expected == 4);
  }

  SECTION("Remove First")
  {
    auto first = new Item<int>(0);

    l.append(new Item<int>(2));
    l.append(new Item<int>(3));
    l.prepend(new Item<int>(1));
    l.prepend(first);

    l.remove(first);

    int expected = 1;

    for(auto i : l)
    {
      CHECK(i->data == expected);
      expected++;
    }

    CHECK(expected == 4);
  }

  SECTION("Remove Last")
  {
    auto last = new Item<int>(3);

    l.append(new Item<int>(2));
    l.append(last);
    l.prepend(new Item<int>(1));
    l.prepend(new Item<int>(0));
    l.remove(last);

    int expected = 0;

    for(auto i : l)
    {
      CHECK(i->data == expected);
      expected++;
    }

    CHECK(expected == 3);
  }

  SECTION("Clear")
  {
    auto last = new Item<int>(3);

    l.append(new Item<int>(2));
    l.append(last);
    l.prepend(new Item<int>(1));
    l.prepend(new Item<int>(0));
    l.remove(last);

    int expected = 0;

    for(auto i : l)
    {
      CHECK(i->data == expected);
      expected++;
    }

    CHECK(expected == 3);
    l.clear();
  }
}