#include "ParallelData.h"
#include <nltools/logging/Log.h>
#include <glib.h>

void testSimdSize()
{
  g_assert((ParallelData<int, 0>::simdSize == 0));
  g_assert((ParallelData<int, 1>::simdSize == 1));
  g_assert((ParallelData<int, 2>::simdSize == 1));
  g_assert((ParallelData<int, 3>::simdSize == 1));
  g_assert((ParallelData<int, 4>::simdSize == 1));
  g_assert((ParallelData<int, 5>::simdSize == 2));
}

void testCompareOperator()
{
  ParallelData<float, 12> a;
  ParallelData<float, 12> b;

  using E = std::tuple<float, float, int>;
  E s[] = { { 30, 20, -1 }, { -10, -9.8, 0 }, { -10, -9.2, -1 }, { 0, 0, 0 },    { 0, 5, -1 },   { 30, 20, -1 },
            { 30, 20, -1 }, { 30, 20, -1 },   { 30, 20, -1 },    { 30, 20, -1 }, { 30, 20, -1 }, { 30, 20, -1 } };

  for(int i = 0; i < 12; i++)
  {
    a[i] = std::get<0>(s[i]);
    b[i] = std::get<1>(s[i]);
  }

  ParallelData<uint32_t, 12> c = std::abs(a - b) > 0.5f;

  for(int i = 0; i < 12; i++)
  {
    g_assert(c[i] == std::get<2>(s[i]));
  }
}

void testRound()
{
  ParallelData<float, 12> a({ -12, -11.6, -11.3, -0.6, -0.2, 0, 12, 11.6, 11.3, 0.6, 0.2, 0 });
  ParallelData<int, 12> b({ -12, -12, -11, -1, 0, 0, 12, 12, 11, 1, 0, 0 });
  auto rounded = std::round(a);
  for(int i = 0; i < 12; i++)
    g_assert(rounded[i] == b[i]);
}

void testABS()
{
  ParallelData<float, 12> a({ -12, -11.6, -11.3, -0.6, -0.2, 0, 12, 11.6, 11.3, 0.6, 0.2, 0 });
  ParallelData<float, 12> b({ 12, 11.6, 11.3, 0.6, 0.2, 0, 12, 11.6, 11.3, 0.6, 0.2, 0 });
  auto rounded = std::abs(a);
  for(int i = 0; i < 12; i++)
    g_assert(rounded[i] == b[i]);
}

void testMin()
{
  using P = ParallelData<float, 4>;
  P a({ -12, 4, 100, 10000 });
  auto m = std::min(a, 1.0f);
  P r({ -12, 1, 1, 1 });

  for(int i = 0; i < 4; i++)
    g_assert(m[i] == r[i]);
}

void testMax()
{
  using P = ParallelData<float, 4>;
  P a({ -12, 4, 100, 10000 });
  auto m = std::max(a, 1.0f);
  P r({ 1, 4, 100, 10000 });

  for(int i = 0; i < 4; i++)
    g_assert(m[i] == r[i]);
}

void testKeepFractional()
{
  ParallelData<float, 12> a({ -12.25f, -11.75f, -11.5f, -0.75f, -0.2f, 0.0f, 12.0f, 11.6f, 11.25f, 0.75f, 0.2f, 0.0f });
  ParallelData<float, 12> b({ -0.25f, 0.25f, 0.5f, 0.25f, -0.2f, 0.0f, 0.0f, -0.4f, 0.25f, -0.25f, 0.2f, 0.0f });
  auto r = keepFractional(a);
  for(int i = 0; i < 12; i++)
  {
    auto diff = std::abs(r[i] - b[i]);
    g_assert(diff < 1e-6f);
  }
}

void testParallelData()
{
  testSimdSize();
  testCompareOperator();
  testRound();
  testABS();
  testMin();
  testMax();
  testKeepFractional();
}
