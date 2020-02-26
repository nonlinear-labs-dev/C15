#pragma once

#include <memory>
#include <ostream>

namespace DescriptiveLayouts
{
  struct Expression
  {
    Expression();
    virtual ~Expression();

    Expression(const Expression&) = delete;
    Expression(Expression&&) = delete;
    void operator=(const Expression&) = delete;
    void operator=(Expression&&) = delete;

    virtual int evaluate() const = 0;
    virtual Expression* clone() const = 0;
    virtual void dump(std::ostream& out) const = 0;
  };

  struct BinaryExpression : Expression
  {
    BinaryExpression(Expression* lhs, Expression* rhs);
    ~BinaryExpression() override;

    Expression* lhs;
    Expression* rhs;
  };

  struct AndExpression : BinaryExpression
  {
    AndExpression(Expression* lhs, Expression* rhs);
    ~AndExpression() override;

    int evaluate() const override;
    Expression* clone() const override;
    void dump(std::ostream& out) const override;
  };

  struct OrExpression : BinaryExpression
  {
    OrExpression(Expression* lhs, Expression* rhs);
    ~OrExpression() override;

    int evaluate() const override;
    Expression* clone() const override;
    void dump(std::ostream& out) const override;
  };

  struct EqExpression : BinaryExpression
  {
    EqExpression(Expression* lhs, Expression* rhs);
    ~EqExpression() override;

    int evaluate() const override;
    Expression* clone() const override;
    void dump(std::ostream& out) const override;
  };

  struct NeqExpression : BinaryExpression
  {
    NeqExpression(Expression* lhs, Expression* rhs);
    ~NeqExpression() override;

    int evaluate() const override;
    Expression* clone() const override;
    void dump(std::ostream& out) const override;
  };

  struct StringExpression : Expression
  {
    StringExpression(const std::string& value);
    ~StringExpression() override;

    int evaluate() const override;
    Expression* clone() const override;
    void dump(std::ostream& out) const override;

    std::string value;
  };

  struct NotExpression : Expression
  {
    NotExpression(Expression* exp);
    ~NotExpression() override;

    int evaluate() const override;
    Expression* clone() const override;
    void dump(std::ostream& out) const override;

    Expression* exp;
  };
}
