#pragma once

#include "Point.h"

class FrameBuffer;

class Rect
{
 public:
  Rect(int x, int y, int w, int h);
  Rect(const Point &leftTop, const Point &rightBottom);
  virtual ~Rect();

  [[nodiscard]] bool contains(int x, int y) const;
  [[nodiscard]] bool intersects(const Rect &other) const;
  [[nodiscard]] Rect getIntersection(const Rect &other) const;

  [[nodiscard]] const Point &getPosition() const;
  [[nodiscard]] int getX() const;
  [[nodiscard]] int getY() const;
  [[nodiscard]] int getTop() const;
  [[nodiscard]] int getLeft() const;
  [[nodiscard]] int getBottom() const;
  [[nodiscard]] int getRight() const;
  [[nodiscard]] int getWidth() const;
  [[nodiscard]] int getHeight() const;

  [[nodiscard]] Point getLeftTop() const;
  [[nodiscard]] Point getCenter() const;

  void setTop(int y);
  void setLeft(int x);
  void setBottom(int w);
  void setRight(int h);
  void setRightBottom(const Point &p);
  void setRightBottom(int b, int r);
  void setWidth(int w);
  void setHeight(int h);
  void moveBy(int x, int y);
  void normalize();

  [[nodiscard]] bool isEmpty() const;

  [[nodiscard]] Rect getMovedBy(const Point &p) const;
  [[nodiscard]] Rect getMargined(int h, int v) const;

  void reduceByMargin(int i);
  void addMargin(int left, int top, int right, int bottom);

  void draw(FrameBuffer &fb) const;
  void drawRounded(FrameBuffer &fb) const;

  friend bool operator==(const Rect &a, const Rect &b);

 private:
  Point m_leftTop;
  int m_width = 0;
  int m_height = 0;
};
