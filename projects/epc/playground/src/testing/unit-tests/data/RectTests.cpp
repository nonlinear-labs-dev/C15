#include <testing/TestHelper.h>
#include <proxies/hwui/controls/Rect.h>

TEST_CASE("Rect Basic", "[Rect]")
{
  Rect a(0, 0, 0, 0);
  CHECK(a.isEmpty());
  CHECK(a.getWidth() == 0);
  CHECK(a.getHeight() == 0);
  CHECK(a.getLeft() == 0);
  CHECK(a.getTop() == 0);
  CHECK(a.getRight() < 0);
  CHECK(a.getBottom() < 0);

  Rect b(0, 0, 1, 1);
  CHECK(!b.isEmpty());
  CHECK(b.getWidth() == 1);
  CHECK(b.getHeight() == 1);
  CHECK(b.getLeft() == 0);
  CHECK(b.getTop() == 0);
  CHECK(b.getRight() == 0);
  CHECK(b.getBottom() == 0);
}

TEST_CASE("Rect Intersect", "[Rect]")
{
  Rect a(0, 0, 10, 10);
  Rect b(20, 20, 10, 10);
  Rect i = a.getIntersection(b);
  CHECK(i.isEmpty());
  CHECK(i.getWidth() == 0);
  CHECK(i.getHeight() == 0);
}