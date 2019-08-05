#include "Point.h"

Point::Point(int x, int y)
    : m_x(x)
    , m_y(y)
{
}

Point::~Point()
{
}

int Point::getX() const
{
  return m_x;
}

int Point::getY() const
{
  return m_y;
}

void Point::setX(int x)
{
  m_x = x;
}

void Point::setY(int y)
{
  m_y = y;
}

void Point::moveBy(int x, int y)
{
  m_x += x;
  m_y += y;
}

bool operator==(const Point &lhs, const Point &rhs)
{
  return lhs.getX() == rhs.getX() && lhs.getY() == rhs.getY();
}

bool operator!=(const Point &lhs, const Point &rhs)
{
  return !(lhs == rhs);
}
