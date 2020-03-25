#include <testing/TestHelper.h>

void assertStrings(const Glib::ustring &query, const std::list<Glib::ustring> &expected)
{
  auto result = SearchQuery::splitQuotes(query);
  CHECK(result == expected);
}

TEST_CASE("Quoted Search Query", "[Search Query]")
{
  assertStrings("", std::list<Glib::ustring> {});

  assertStrings("abc \"def\" ghi", std::list<Glib::ustring> { "abc", "def", "ghi" });

  assertStrings("abc \"def ghi", std::list<Glib::ustring> { "abc", "\"def", "ghi" });

  assertStrings("abc    def   ghi", std::list<Glib::ustring> { "abc", "def", "ghi" });

  assertStrings("\"abc    def   ghi\"", std::list<Glib::ustring> { "abc    def   ghi" });

  assertStrings(R"("aaa ""bbb")", std::list<Glib::ustring> { "aaa", "bbb" });

  assertStrings(R"("aaa ""bbb bbb")", std::list<Glib::ustring> { "aaa", "bbb bbb" });

  assertStrings(R"("aaa ""bbb ccc)", std::list<Glib::ustring> { "aaa", "\"bbb", "ccc" });
}