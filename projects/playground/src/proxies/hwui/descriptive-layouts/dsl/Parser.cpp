#include "Parser.h"
#include "Expression.h"
#include <nltools/logging/Log.h>
#include <stack>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/spirit/include/qi_auto.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phx = boost::phoenix;

using ascii::char_;
using ascii::string;
using boost::spirit::ascii::space;
using qi::double_;
using qi::int_;
using qi::lexeme;
using qi::lit;
using iterator_type = std::string::const_iterator;

template <typename T = std::string> using Rule = qi::rule<iterator_type, T(), ascii::space_type>;

namespace DescriptiveLayouts
{
  template <typename TStyle> struct StyleRule
  {
    using Style = TStyle;
    std::string name;
  };

  Parser::Parser()
  {
  }

  static Element::Strings addPrefix(const Element::Strings& in, const std::string& prefix)
  {
    Element::Strings out;
    std::transform(in.begin(), in.end(), std::back_inserter(out), [&](auto& a) { return prefix + a; });
    return out;
  }

  static boost::spirit::qi::symbols<char, std::string> stringsToRule(const Element::Strings& items)
  {
    return boost::spirit::qi::symbols<char, std::string>(items, items);
  }

  template <typename E> static boost::spirit::qi::symbols<char, E> stringsToEnumRule()
  {
    return boost::spirit::qi::symbols<char, E>(StyleValues::getAllStrings<E>(), StyleValues::getAllValues<E>());
  }

  template <int StyleNumber> struct StyleSetter
  {
    StyleSetter(Element*& e)
        : e(e)
    {
    }

    template <typename P> void operator()(const P& p) const
    {
      std::get<StyleNumber>(e->style) = p;
    }

    Element*& e;
  };

  Element Parser::parseString(const std::string& content)
  {
    nltools::Log::warning("Parsing: '" + content + "'");

    // clang-format off
    Element root(nullptr, "root");
    Element* current = &root;
    std::stack<Element*> stack;
    stack.push(&root);

    auto onRef = [&](auto s) { current->refersTo = s; };
    auto onIf = [&](auto s) { current->conditions.reset(s); };
    auto onX = [&](int x) { current->position.setLeft(x); };
    auto onY = [&](int y) { current->position.setTop(y); };
    auto onWidth = [&](int w) { current->position.setWidth(w); };
    auto onHeight = [&](int h) { current->position.setHeight(h); };
    auto onInit = [&](auto s) { current->inits.push_back(s); };

    auto onFire = [&](auto s) { current->fires.push_back(s); };
    auto onReaction = [&](auto s) { current->reactions.push_back(s); };
    auto onUses = [&](auto s) { current->eventProvider = to<EventProviders>(s); };
    auto diveIn = [&](auto) { stack.push(current); };
    auto diveOut = [&](auto) {
      stack.pop();
      current = stack.top();
    };
    auto newElement = [&](auto s) { current = &(stack.top()->children.emplace_back(stack.top(), s)); };

    Rule<> element;

    auto delim = string("/");
    auto comma = string(",");
    Rule<void> point = '(' >> int_[onX] >> comma >> int_[onY] >> ')';
    Rule<void> rect
        = '(' >> int_[onX] >> comma >> int_[onY] >> comma >> int_[onWidth] >> comma >> int_[onHeight] >> ')';
    Rule<> identifier = lexeme[qi::alpha >> +qi::alnum];

    auto primitiveProperty = stringsToRule(DescriptiveLayouts::getAllStrings<PrimitiveProperty>());
    auto uiFocusOptions = stringsToRule(::getAllStrings<UIFocus>());
    auto uiModeOptions = stringsToRule(::getAllStrings<UIMode>());
    auto uiDetailOptions = stringsToRule(::getAllStrings<UIDetail>());
    auto primitives = stringsToRule(addPrefix(DescriptiveLayouts::getAllStrings<PrimitiveClasses>(), "@"));
    auto eventSourceName = stringsToRule(DescriptiveLayouts::getAllStrings<EventSources>());
    auto eventSinkName = stringsToRule(DescriptiveLayouts::getAllStrings<EventSinks>());
    auto buttonName = stringsToRule(::getAllStrings<Buttons>());

    Rule<> children = lit("{")[diveIn] >> *element >> lit('}')[diveOut];
    Rule<> referencedElement;
    referencedElement = identifier >> -(delim >> referencedElement);
    Rule<> reference = referencedElement | primitives;

    // if
    auto openBrace = qi::ascii::char_('(');
    auto closeBrace = qi::ascii::char_(')');

    using ExpRule = qi::rule<std::string::const_iterator, Expression*(), ascii::space_type>;

    ExpRule orExpr;
    ExpRule andExpr;
    ExpRule eqExpr;
    ExpRule neqExpr;
    ExpRule notExpr;
    ExpRule condExpr;
    ExpRule condExprGrp;

    orExpr = (andExpr >> "||" >> orExpr)[qi::_val = phx::new_<OrExpression>(qi::_1, qi::_2)] | andExpr[qi::_val = qi::_1];
    andExpr = (eqExpr >> "&&" >> andExpr)[qi::_val = phx::new_<AndExpression>(qi::_1, qi::_2)] | eqExpr[qi::_val = qi::_1];
    eqExpr = (neqExpr >> "==" >> eqExpr)[qi::_val = phx::new_<EqExpression>(qi::_1, qi::_2)] | neqExpr[qi::_val = qi::_1];
    neqExpr = (notExpr >> "!=" >> neqExpr)[qi::_val = phx::new_<NeqExpression>(qi::_1, qi::_2)] | notExpr[qi::_val = qi::_1];
    notExpr = ("!" >> condExpr)[qi::_val = phx::new_<NotExpression>(qi::_1)] | condExpr[qi::_val = qi::_1];
    condExpr = condExprGrp[qi::_val = qi::_1] | identifier[qi::_val = phx::new_<StringExpression>(qi::_1)];
    condExprGrp = openBrace >> orExpr[qi::_val = qi::_1] >> closeBrace;
    // fi

    Rule<> primitiveValue = lexeme['"' >> +(char_ - '"') >> '"'];

    Rule<> reaction = eventSourceName >> string(":") >> identifier >> string("[") >> primitiveProperty >> string("]");
    Rule<> init = (identifier >> string("[") >> primitiveProperty >> string("]") >> string("=") >> primitiveValue) | (primitiveProperty >> string("=") >> primitiveValue);
    Rule<> fire = buttonName >> string("=") >> eventSinkName;

    auto colors = stringsToEnumRule<StyleValues::Color>();
    auto alignments = stringsToEnumRule<StyleValues::Alignment>();
    auto borders = stringsToEnumRule<StyleValues::BorderStyle>();
    auto fonts = stringsToEnumRule<StyleValues::Font>();

    Rule<StyleValues::Color> colorStyle = lit("Color") >> lit("=") >> colors;
    Rule<StyleValues::Color> backgroundColorStyle = lit("BackgroundColor") >> lit("=") >> colors;
    Rule<StyleValues::Color> suffixColorStyle = lit("SuffixColor") >> lit("=") >> colors;
    Rule<StyleValues::Color> borderColorStyle = lit("BorderColor") >> lit("=") >> colors;
    Rule<StyleValues::Color> highlightBackgroundColorStyle = lit("HighlightBackgroundColor") >> lit("=") >> colors;
    Rule<StyleValues::Color> highlightColorStyle = lit("HighlightColor") >> lit("=") >> colors;
    Rule<StyleValues::Alignment> alignmentStyle = lit("Alignment") >> lit("=") >> alignments;
    Rule<StyleValues::BorderStyle> borderStyle = lit("BorderStyle") >> lit("=") >> borders;
    Rule<StyleValues::Font> fontStyle = lit("Font") >> lit("=") >> fonts;
    Rule<int> fontSize = lit("FontSize") >> lit("=") >> int_;

    Rule<> enumStyleRules = colorStyle[StyleSetter<0>(current)] | backgroundColorStyle[StyleSetter<1>(current)]
        | suffixColorStyle[StyleSetter<2>(current)] | borderColorStyle[StyleSetter<3>(current)]
        | highlightBackgroundColorStyle[StyleSetter<4>(current)] | highlightColorStyle[StyleSetter<5>(current)]
        | alignmentStyle[StyleSetter<6>(current)] | borderStyle[StyleSetter<7>(current)]
        | fontStyle[StyleSetter<8>(current)] | fontSize[StyleSetter<9>(current)];

    Rule<> fontSizeRule = string("FontSize") >> string("=") >> qi::int_;
    Rule<> style = enumStyleRules | fontSizeRule;

    Rule<> ofRule = lit("of") >> (reference[onRef] | '(' >> reference[onRef] >> ')');
    Rule<> ifRule = lit("if") >> condExprGrp[onIf];
    Rule<> atRule = lit("at") >> (point | rect);
    Rule<> usesRule = lit("uses") >> (identifier[onUses] | '(' >> identifier[onUses] >> ')');
    Rule<> reactRule = lit("react") >> (reaction[onReaction] | '(' >> +reaction[onReaction] >> ')');
    Rule<> initRule = lit("init") >> (init[onInit] | '(' >> +init[onInit] >> ')');
    Rule<> styleRule = lit("style") >> (style | '(' >> +style >> ')');
    Rule<> fireRule = lit("fire") >> (fire[onFire] | '(' >> +fire[onFire] >> ')');
    Rule<> rule = ofRule | atRule | ifRule | usesRule | reactRule | initRule | fireRule | styleRule;

    element = identifier[newElement] >> *rule >> *children;

    qi::rule<iterator_type, ascii::space_type> ui = +element;
    qi::grammar<iterator_type, ascii::space_type> g(ui);

    std::string::const_iterator iter = content.begin();
    std::string::const_iterator end = content.end();

    if(phrase_parse(iter, end, g, boost::spirit::ascii::space) && iter == end)
      return root;
    else
      nltools::Log::warning("String contains crap:", std::string(iter, end));

    nltools::Log::warning("String contains crap:", content);
    throw std::runtime_error("Could not parse file completely: '" + content + "'");
    // clang-format on
  }
}
