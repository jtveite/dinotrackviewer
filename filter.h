#pragma once

#include "vrpoint.h"


/*
 *  The Filter class represents a filter for points to view only a subset
 *  Default class is to show all points.
 */
class Filter {
public:
  Filter(){};
  /*
   *  Checks if a point should be visible or not.
   *  Returns true to show the point, false to hide.
   */
  virtual bool checkPoint(VRPoint& p);
};

class MotionFilter : public Filter {
public:
  MotionFilter();
  MotionFilter(float threshold);
  virtual bool checkPoint(VRPoint& p);
private:
  float threshold;
};


class SliceFilter : public Filter{
public:
  //SliceFilter();
  SliceFilter(float start = 0, float gap = .04, float size = .005);
  virtual bool checkPoint(VRPoint& p);
  void addStart(float amt);
  void addGap(float amt);
  void addSize(float amt);

private:
  float start, gap, size;
};
