#pragma once

class Point
{
 public:
  Point(int x, int y);
  virtual ~Point();

  int getX() const;
  int getY() const;
  void moveBy(int x, int y);

  void setX(int x);
  void setY(int y);

 private:
  int m_x;
  int m_y;
};

inline Point operator+(const Point &one, const Point &other)
{
  return Point(one.getX() + other.getX(), one.getY() + other.getY());
}
