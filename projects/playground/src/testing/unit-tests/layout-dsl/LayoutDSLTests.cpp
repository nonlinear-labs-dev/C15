#include <testing/TestHelper.h>
#include <third-party/include/catch.hpp>
#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/descriptive-layouts/dsl/Expression.h>
#include <proxies/hwui/descriptive-layouts/dsl/Parser.h>
#include <proxies/hwui/descriptive-layouts/dsl/Algorithm.h>
#include <sstream>
#include <iostream>

using namespace DescriptiveLayouts;

void assertExpression(const std::string &expression, const std::string &expectedDump)
{
  Element flat = flatten(Parser::parseString("TheElement of @Bar if " + expression));
  REQUIRE(flat.children.size() == 1);
  std::stringstream str;
  flat.children[0].conditions->dump(str);
  std::cout << str.str() << std::endl;
  REQUIRE(str.str() == expectedDump);
}

TEST_CASE("dsl-layouts")
{
  WHEN("simplest element")
  {
    auto e = Parser::parseString("TheElement");

    THEN("one element")
    {
      REQUIRE(e.children.size() == 1);
      REQUIRE(e.children[0].name == "TheElement");
    }
  }

  WHEN("simplestElement - resolve")
  {
    Element e = Parser::parseString("TheElement");
    Element flat = flatten(e);

    THEN("one element")
    {
      REQUIRE(e.children.size() == 1);
      REQUIRE(flat.children.size() == 0);
    }
  }

  WHEN("simpleElement - eventProvider - global")
  {
    auto e = Parser::parseString("TheElement uses Global");
    REQUIRE(e.children[0].eventProvider == EventProviders::Global);
  }

  WHEN("simpleElement/position/point")
  {
    auto e = Parser::parseString("TheElement at (12, 34)");
    REQUIRE(e.children[0].position == Rect(12, 34, Element::maxPos, Element::maxPos));
  }

  WHEN("simpleElement/position/rect")
  {
    auto e = Parser::parseString("TheElement at (12, 34, 56, 78)");
    REQUIRE(e.children[0].position == Rect(12, 34, 56, 78));
  }

  WHEN("nestedElement/position-moved")
  {
    Element flat = flatten(
        Parser::parseString("TheElement at (12, 34, 500, 600) { NestedElement of @Bar at (100, 100, 20, 30) }"));
    REQUIRE(flat.children.size() == 1);
    REQUIRE(flat.children[0].position == Rect(112, 134, 20, 30));
    REQUIRE(flat.children[0].name == "/root/TheElement/NestedElement");
  }

  WHEN("nestedElement/position-clamped-1")
  {
    Element flat
        = flatten(Parser::parseString("TheElement at (10, 10) { NestedElement of @Bar at (20, 20, 100, 100) }"));
    REQUIRE(flat.children.size() == 1);
    REQUIRE(flat.children[0].position == Rect(30, 30, 100, 100));
    REQUIRE(flat.children[0].name == "/root/TheElement/NestedElement");
  }

  WHEN("nestedElement/position-clamped-2")
  {
    Element flat = flatten(
        Parser::parseString("TheElement at (10, 10, 50, 50) { NestedElement of @Bar at (20, 20, 100, 100) }"));
    REQUIRE(flat.children.size() == 1);
    REQUIRE(flat.children[0].position == Rect(30, 30, 30, 30));
    REQUIRE(flat.children[0].name == "/root/TheElement/NestedElement");
  }

  WHEN("styles/Color")
  {
    Element flat = flatten(Parser::parseString("TheElement of @Bar style (Color=C103)"));
    REQUIRE(flat.children.size() == 1);
    REQUIRE(std::get<0>(flat.children[0].style) == StyleValues::Color::C103);
  }

  WHEN("nested-primitives")
  {
    Element flat = flatten(Parser::parseString("TheElement of @Bar { NestedElement of @Text }"));
    REQUIRE(flat.children.size() == 2);
  }

  WHEN("styles/Color-nested")
  {
    Element flat = flatten(Parser::parseString("TheElement of @Bar style (Color=C103) { NestedElement of @Text }"));
    REQUIRE(flat.children.size() == 2);
    REQUIRE(std::get<0>(flat.children[0].style) == StyleValues::Color::C103);
    REQUIRE(std::get<0>(flat.children[1].style) == StyleValues::Color::C103);
  }

  WHEN("styles/Color-nested-overriding")
  {
    Element flat = flatten(
        Parser::parseString("TheElement of @Bar style (Color=C103) { NestedElement of @Text style (Color=C128)}"));
    REQUIRE(flat.children.size() == 2);
    REQUIRE(std::get<0>(flat.children[0].style) == StyleValues::Color::C103);
    REQUIRE(std::get<0>(flat.children[1].style) == StyleValues::Color::C128);
  }

  WHEN("styles/multiple")
  {
    Element flat = flatten(Parser::parseString(
        "TheElement of @Bar style (Color=C103 BackgroundColor=C128) { NestedElement of @Text style (Font=Bold)}"));
    REQUIRE(flat.children.size() == 2);
    REQUIRE(std::get<0>(flat.children[0].style) == StyleValues::Color::C103);
    REQUIRE(std::get<1>(flat.children[0].style) == StyleValues::Color::C128);
    REQUIRE(std::get<8>(flat.children[1].style) == StyleValues::Font::Bold);
  }

  WHEN("styles/fontsize")
  {
    Element flat = flatten(Parser::parseString("TheElement of @Bar style (FontSize=12)"));
    REQUIRE(flat.children.size() == 1);
    REQUIRE(std::get<9>(flat.children[0].style) == 12);
  }

  WHEN("references")
  {
    auto def = "A1 { B1 { C1 of A2 }} "
               "A2 { B2 { C2 of @Bar }} ";
    auto parsed = Parser::parseString(def);
    REQUIRE(resolveReferences(parsed));
    REQUIRE(parsed.children.size() == 2);
    REQUIRE(parsed.children[0].children.size() == 1);
    REQUIRE(parsed.children[1].children.size() == 1);
    REQUIRE(parsed.children[0].children[0].children[0].children[0].children[0].refersTo == "@Bar");
    REQUIRE(parsed.children[1].children[0].children[0].refersTo == "@Bar");
  }

  WHEN("dsl-expressions")
  {
    assertExpression("(UIFocus == Parameters)", "(UIFocus EQ Parameters)");
    assertExpression("(a == b)", "(a EQ b)");
    assertExpression("(a == b && b == c && c == d)", "((a EQ b) AND ((b EQ c) AND (c EQ d)))");
    assertExpression("(a != b)", "(a NEQ b)");
    assertExpression("(a == b && c != d)", "((a EQ b) AND (c NEQ d))");
    assertExpression("(a == b || c != d)", "((a EQ b) OR (c NEQ d))");
    assertExpression("(multipleLetters == b || c != word)", "((multipleLetters EQ b) OR (c NEQ word))");
    assertExpression("(multipleLetters==b||c!= word)", "((multipleLetters EQ b) OR (c NEQ word))");
    assertExpression("(a == (b != c))", "(a EQ (b NEQ c))");
    assertExpression("((a == b) != c)", "((a EQ b) NEQ c)");
    assertExpression("(a && b)", "(a AND b)");
    assertExpression("(a || b)", "(a OR b)");
    assertExpression("(a || (b && c))", "(a OR (b AND c))");
    assertExpression("((a || b) && c)", "((a OR b) AND c)");
    assertExpression("((a||b)&&c)", "((a OR b) AND c)");
    assertExpression("(!(a == b))", "(NOT (a EQ b))");
    assertExpression("(!(a != b && a == b))", "(NOT ((a NEQ b) AND (a EQ b)))");
    assertExpression("(!(a != b && a == b) && (x == y))", "((NOT ((a NEQ b) AND (a EQ b))) AND (x EQ y))");
  }

  WHEN("condition")
  {
    Element flat = flatten(Parser::parseString("TheElement of @Bar if (UIFocus == Parameters)"));
    REQUIRE(flat.children.size() == 1);
    std::stringstream str;
    flat.children[0].conditions->dump(str);
    std::cout << str.str() << std::endl;
    REQUIRE(str.str() == "(UIFocus EQ Parameters)");
  }

  WHEN("nested condition")
  {
    Element flat = flatten(
        Parser::parseString("TheElement of @Bar if (UIFocus == Parameters) { Nested of @Bar if (UIMode == Select) }"));
    REQUIRE(flat.children.size() == 2);

    {
      std::stringstream str;
      flat.children[0].conditions->dump(str);
      REQUIRE(str.str() == "(UIFocus EQ Parameters)");
    }

    {
      std::stringstream str;
      flat.children[1].conditions->dump(str);
      REQUIRE(str.str() == "((UIFocus EQ Parameters) AND (UIMode EQ Select))");
    }
  }

  WHEN("execute condition true")
  {
    Application::get().getHWUI()->setFocusAndMode(UIFocus::Parameters);
    Element flat = flatten(Parser::parseString("TheElement of @Bar if (UIFocus == Parameters)"));
    REQUIRE(flat.children[0].conditions->evaluate() == 1);
  }

  WHEN("execute condition false")
  {
    Application::get().getHWUI()->setFocusAndMode(UIFocus::Presets);
    Element flat = flatten(Parser::parseString("TheElement of @Bar if (UIFocus == Parameters)"));
    REQUIRE(flat.children[0].conditions->evaluate() == 0);
  }
}
