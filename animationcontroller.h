#pragma once


#include <chrono>



class AnimationController
{

public:
  AnimationController(int numFrames);
  AnimationController();
  void setFrameCount(int numFrames);

  int getFrame();
  
  void setSpeed(float speed);
  float getSpeed();
  void increaseSpeed();
  void decreaseSpeed();


private:
  float speed;
  int frameCount;
  int currFrame;
  std::chrono::high_resolution_clock::time_point startTime;

  void ValidateSpeed();
};
