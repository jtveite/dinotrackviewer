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

  void play();
  void pause();
  void togglePlay();

  void stepForward();
  void stepBackward();


private:
  const float speedMultiplier = 1.05;
  float speed;
  int frameCount;
  int currFrame;
  bool playing = true;
  std::chrono::high_resolution_clock::time_point startTime;

  void ValidateSpeed();
};
