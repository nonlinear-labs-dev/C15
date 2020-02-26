#include "Expression.h"
#include <unordered_map>
#include <functional>

#include <Application.h>
#include <proxies/hwui/HWUI.h>

namespace DescriptiveLayouts
{

  Expression::Expression()
  {
  }

  Expression::~Expression()
  {
  }

  BinaryExpression::BinaryExpression(Expression* lhs, Expression* rhs)
      : lhs(std::move(lhs))
      , rhs(std::move(rhs))
  {
  }

  BinaryExpression::~BinaryExpression()
  {
    delete lhs;
    delete rhs;
  }

  AndExpression::AndExpression(Expression* lhs, Expression* rhs)
      : BinaryExpression(lhs, rhs)
  {
  }

  int AndExpression::evaluate() const
  {
    return lhs->evaluate() && rhs->evaluate();
  }

  AndExpression::~AndExpression() = default;

  Expression* AndExpression::clone() const
  {
    return new AndExpression(lhs->clone(), rhs->clone());
  }

  void AndExpression::dump(std::ostream& out) const
  {
    out << "(";
    lhs->dump(out);
    out << " AND ";
    rhs->dump(out);
    out << ")";
  }

  OrExpression::OrExpression(Expression* lhs, Expression* rhs)
      : BinaryExpression(lhs, rhs)
  {
  }

  int OrExpression::evaluate() const
  {
    return lhs->evaluate() || rhs->evaluate();
  }

  OrExpression::~OrExpression() = default;

  void OrExpression::dump(std::ostream& out) const
  {
    out << "(";
    lhs->dump(out);
    out << " OR ";
    rhs->dump(out);
    out << ")";
  }

  Expression* OrExpression::clone() const
  {
    return new OrExpression(lhs->clone(), rhs->clone());
  }

  EqExpression::EqExpression(Expression* lhs, Expression* rhs)
      : BinaryExpression(lhs, rhs)
  {
  }

  int EqExpression::evaluate() const
  {
    return lhs->evaluate() == rhs->evaluate();
  }

  EqExpression::~EqExpression() = default;

  Expression* EqExpression::clone() const
  {
    return new EqExpression(lhs->clone(), rhs->clone());
  }

  void EqExpression::dump(std::ostream& out) const
  {
    out << "(";
    lhs->dump(out);
    out << " EQ ";
    rhs->dump(out);
    out << ")";
  }

  NeqExpression::NeqExpression(Expression* lhs, Expression* rhs)
      : BinaryExpression(lhs, rhs)
  {
  }

  int NeqExpression::evaluate() const
  {
    return lhs->evaluate() != rhs->evaluate();
  }

  NeqExpression::~NeqExpression() = default;

  Expression* NeqExpression::clone() const
  {
    return new NeqExpression(lhs->clone(), rhs->clone());
  }

  void NeqExpression::dump(std::ostream& out) const
  {
    out << "(";
    lhs->dump(out);
    out << " NEQ ";
    rhs->dump(out);
    out << ")";
  }

  StringExpression::StringExpression(const std::string& value)
      : value(value)
  {
  }

  static std::unordered_map<std::string, int> getStringConstantsMap()
  {
    std::unordered_map<std::string, int> ret;

    for(auto a : getAllValues<UIFocus>())
      ret[toString(a)] = static_cast<int>(a);

    for(auto a : getAllValues<UIMode>())
      ret[toString(a)] = static_cast<int>(a);

    for(auto a : getAllValues<UIDetail>())
      ret[toString(a)] = static_cast<int>(a);

    return ret;
  }

  int StringExpression::evaluate() const
  {
    static std::unordered_map<std::string, std::function<int()>> getters {
      { "UIFocus", [] { return static_cast<int>(Application::get().getHWUI()->getFocusAndMode().focus); } },
      { "UIMode", [] { return static_cast<int>(Application::get().getHWUI()->getFocusAndMode().mode); } },
      { "UIDetail", [] { return static_cast<int>(Application::get().getHWUI()->getFocusAndMode().detail); } }
    };

    static std::unordered_map<std::string, int> constants = getStringConstantsMap();

    auto itc = constants.find(value);
    if(itc != constants.end())
      return itc->second;

    auto itg = getters.find(value);
    if(itg != getters.end())
      return itg->second();

    nltools::throwException("Could not evaluate string expression: " + value);
  }

  StringExpression::~StringExpression() = default;

  Expression* StringExpression::clone() const
  {
    return new StringExpression(value);
  }

  void StringExpression::dump(std::ostream& out) const
  {
    out << value;
  }

  NotExpression::NotExpression(Expression* exp)
      : exp(exp)
  {
  }

  NotExpression::~NotExpression()
  {
    delete exp;
  }

  int NotExpression::evaluate() const
  {
    return exp->evaluate() ? 0 : 1;
  }

  Expression* NotExpression::clone() const
  {
    return new NotExpression(exp->clone());
  }

  void NotExpression::dump(std::ostream& out) const
  {
    out << "(NOT ";
    exp->dump(out);
    out << ")";
  }
}
