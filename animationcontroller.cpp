#include "animationcontroller.h"
#include <cstdio>

AnimationController::AnimationController(){
  speed = 1;
  frameCount = 50;
  currFrame = 0;
}

AnimationController::AnimationController(int numFrames){
  speed = 1;
  frameCount = numFrames;
  currFrame = 0;
}

void AnimationController::setFrameCount(int numFrames){
  if (currFrame >= numFrames){
    currFrame = numFrames - 1;
  }
  frameCount = numFrames;
}

int AnimationController::getFrame(){
  using namespace std::chrono;
  high_resolution_clock::time_point currentTime = high_resolution_clock::now();
  duration<double> time_passed = duration_cast<duration<double>>(currentTime - startTime);
  //printf("Time passed: %f\n", time_passed.count());
  if (time_passed.count() > (1.0/speed)){
    currFrame++;
    startTime = currentTime;
  }
  if (currFrame >= frameCount){
    currFrame = 0;
  }
  return currFrame;
}

void AnimationController::setSpeed(float newSpeed){
  speed = newSpeed;
  ValidateSpeed();
}

float AnimationController::getSpeed(){
  return speed;
}

void AnimationController::increaseSpeed(){
  speed++;
  ValidateSpeed();
}

void AnimationController::decreaseSpeed(){
  speed--;
  ValidateSpeed();
}

void AnimationController::ValidateSpeed(){
  if (speed <= 0){
    speed = 0.01;
  }
}
