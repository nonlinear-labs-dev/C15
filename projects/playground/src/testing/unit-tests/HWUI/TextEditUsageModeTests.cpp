#include <testing/TestHelper.h>
#include <proxies/hwui/TextEditUsageMode.h>

TEST_CASE("TextEditUsageMode", "[HWUI")
{
  TextEditUsageMode mode("");
  g_assert_true(mode.getLayout() == TextEditUsageMode::Layout::Normal);

  mode.toggleCapsLock();
  g_assert_true(mode.getLayout() == TextEditUsageMode::Layout::Shift);

  mode.toggleCapsLock();
  g_assert_true(mode.getLayout() == TextEditUsageMode::Layout::Normal);

  mode.toggleCapsLock();
  g_assert_true(mode.getLayout() == TextEditUsageMode::Layout::Shift);

  mode.toggleSymbol();
  g_assert_true(mode.getLayout() == TextEditUsageMode::Layout::SymbolShift);

  mode.toggleSymbol();
  g_assert_true(mode.getLayout() == TextEditUsageMode::Layout::Shift);

  mode.toggleSymbol();
  g_assert_true(mode.getLayout() == TextEditUsageMode::Layout::SymbolShift);

  mode.toggleCapsLock();
  g_assert_true(mode.getLayout() == TextEditUsageMode::Layout::Symbol);

  mode.toggleSymbol();
  g_assert_true(mode.getLayout() == TextEditUsageMode::Layout::Normal);

  mode.handleShiftButton(true);
  g_assert_true(mode.getLayout() == TextEditUsageMode::Layout::Shift);

  mode.handleShiftButton(false);
  g_assert_true(mode.getLayout() == TextEditUsageMode::Layout::Normal);

  mode.toggleCapsLock();
  g_assert_true(mode.getLayout() == TextEditUsageMode::Layout::Shift);

  mode.handleShiftButton(true);
  g_assert_true(mode.getLayout() == TextEditUsageMode::Layout::Normal);

  mode.handleShiftButton(false);
  g_assert_true(mode.getLayout() == TextEditUsageMode::Layout::Shift);
}