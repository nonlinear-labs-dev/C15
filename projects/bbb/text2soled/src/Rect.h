#pragma once

#include "Point.h"

class FrameBuffer;

class Rect
{
 public:
  Rect(int x, int y, int w, int h);
  Rect(const Point &leftTop, const Point &rightBottom);
  virtual ~Rect();

  bool contains(int x, int y) const;
  Rect getIntersection(const Rect &other) const;

  const Point &getPosition() const;
  int getX() const;
  int getY() const;
  int getTop() const;
  int getLeft() const;
  int getBottom() const;
  int getRight() const;
  int getWidth() const;
  int getHeight() const;

  Point getCenter() const;

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

  bool isEmpty() const;

  Rect getMovedBy(const Point &p) const;
  Rect getMargined(int h, int v) const;

  void reduceByMargin(int i);
  void addMargin(int left, int top, int right, int bottom);

  void drawRounded(FrameBuffer &fb) const;

 private:
  Point m_leftTop;
  int m_width = 0;
  int m_height = 0;
};
