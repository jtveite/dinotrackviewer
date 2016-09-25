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
  double currentTime = 0;
  if (playing){
    double time_passed = currentTime - startTime;
    if (time_passed > (1.0/speed)){
      currFrame++;
      startTime = currentTime;
    }
    if (currFrame >= frameCount){
      currFrame = 0;
    }
  }
  printf("Frame time : %f \n", currentTime - lastTime);
  lastTime = currentTime;
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
  speed *= speedMultiplier;
  ValidateSpeed();
}

void AnimationController::decreaseSpeed(){
  speed /= speedMultiplier;
  ValidateSpeed();
}

void AnimationController::ValidateSpeed(){
  if (speed <= 0){
    speed = 0.01;
  }
}

void AnimationController::play(){
  playing = true;
}

void AnimationController::pause(){
  playing = false;
}

void AnimationController::togglePlay(){
  playing = !playing;
}

void AnimationController::stepForward(){
  currFrame++;
  if (currFrame >= frameCount){
    currFrame = 0;
  }
}


void AnimationController::stepBackward(){
  currFrame--;
  if (currFrame < 0){
    currFrame = frameCount - 1;
  }
}
