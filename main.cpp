#include <vrg3d/VRG3D.h>
#include "pointmanager.h"
#include "animationcontroller.h"


#define PROFILING
#undef PROFILING

#ifdef PROFILING
#include "gperftools/profiler.h"
#endif

using namespace G3D;

class DinoApp : public VRApp
{
public:
  DinoApp(std::string setup, std::string dataFile) : VRApp(),
  ac()
  {
    Log *dinolog = new Log("dino-log.txt");
    init(setup, dinolog);
    _mouseToTracker = new MouseToTracker(getCamera(), 2);
    frameTime = clock();

//    pm.ReadFile("/users/jtveite/data/jtveite/slices-130.out");
//    pm.ReadFile("/users/jtveite/dinotrackviewer/test-data");
    pm.ReadFile(dataFile);
    Matrix3 scale = Matrix3::fromDiagonal(Vector3(20, 20, 20));
    CoordinateFrame scaleC (scale);
    CoordinateFrame rotate = CoordinateFrame::fromXYZYPRDegrees(
      1, 0, -7,
      0, -90, 180);
    _owm = rotate * scale;
    ac.setFrameCount(50);//Change to dynamically check how many frames in PM
    ac.setSpeed(15);

    //std::cout << GL_NO_ERROR << ' ' << GL_INVALID_ENUM << ' ' << GL_INVALID_VALUE << ' ' << GL_INVALID_OPERATION << ' ' << GL_INVALID_FRAMEBUFFER_OPERATION << std::endl;
    //
    std::string profFile = "/users/jtveite/d/prof/profile-" + setup;
#ifdef PROFILING
    ProfilerStart(profFile.c_str());
#endif
  }

  virtual ~DinoApp() {
#ifdef PROFILING
    ProfilerFlush();
    ProfilerStop();
#endif
  }

  //general skeleton of user input from demo app
  void doUserInput(Array<VRG3D::EventRef> &events)
  {

    Array<VRG3D::EventRef> newEvents;
    _mouseToTracker->doUserInput(events, newEvents);
    events.append(newEvents);

    for (int i = 0; i < events.size(); i++) {

      std::string eventName = events[i]->getName();
      if (endsWith(events[i]->getName(), "_Tracker")) {
        if (_trackerFrames.containsKey(events[i]->getName())){
          _trackerFrames[events[i]->getName()] = events[i]->getCoordinateFrameData(); 
        }
        else {
          _trackerFrames.set(events[i]->getName(), events[i]->getCoordinateFrameData());
        }
      }
      else if(eventName == "Mouse_Left_Btn_down"){
        _moving = true;
      }
      else if(eventName == "Mouse_Left_Btn_up"){
        _moving = false;
      }
      else if (eventName == "Wand_Right_Btn_down"){
        _moving = true;
      }
      else if (eventName == "Wand_Right_Btn_up"){
        _moving = false;
      }
      else if (eventName == "kbd_LEFT_down"){
        ac.decreaseSpeed();
      }
      else if (eventName == "kbd_RIGHT_down"){
        ac.increaseSpeed();
      }
      else if (eventName == "B04_down"){
        ac.decreaseSpeed();
      }
      else if (eventName == "B03_down"){
        ac.increaseSpeed();
      }
      
      else if (eventName == "B05_down"){
        ac.stepBackward();
      }
      else if (eventName == "B06_down"){
        ac.stepForward();
      }
      else if (eventName == "B07_down"){
        ac.togglePlay();
      }



      else if (beginsWith(eventName, "aimo")){}
      else if (eventName == "SynchedTime"){}
      else{
      //  std::cout << eventName << std::endl;
      }



    }
    
  }
 

  void doGraphics(RenderDevice *rd)
  {
    //std::cout << "Rendering frame: " << _frameCounter << std::endl;

    //Do rotation of trackers if needed
    //std::string targetTracker = "Mouse1_Tracker";
    std::string targetTracker = "Wand_Tracker";
    Array<std::string> trackerNames = _trackerFrames.getKeys();
    for (int i = 0; i < trackerNames.size(); i++){
      CoordinateFrame trackerFrame = _trackerFrames[trackerNames[i]];
      if (trackerNames[i] == targetTracker){
        if (_moving){
          CoordinateFrame amountMoved = trackerFrame * _lastTrackerLocation.inverse();
          _owm = amountMoved * _owm;
        }
        _lastTrackerLocation = trackerFrame;
      }

    }

    rd->pushState();
    //rd->setObjectToWorldMatrix(_virtualToRoomSpace);
    int gl_error;
    while((gl_error = glGetError()) != GL_NO_ERROR)
    {

      std::cout << "Flushing gl errors " << gl_error << std::endl;
    }
    float t = ac.getFrame();
   
    rd->pushState();
    rd->setObjectToWorldMatrix(_owm);
    Matrix4 mvp = rd->invertYMatrix() * rd->modelViewProjectionMatrix();
    
    //std::cout << t << std::endl;
    pm.Draw(rd, t, mvp);
    rd->popState();

    Vector3 hp = getCamera()->getHeadPos();
   // printf("Head position: %f, %f, %f\n", hp.x, hp.y, hp.z);
    Vector3 lv = getCamera()->getLookVec();
  //  printf("Look Vector: %f, %f, %f\n", lv.x, lv.y, lv.z);

    CoordinateFrame owm = rd->modelViewMatrix();
  //  std::cout << owm.toXML() << std::endl;
    rd->popState();
    clock_t newTime = clock();
    //printf("Total frame time was %f seconds.\n", ((float) (newTime - frameTime)) / CLOCKS_PER_SEC);
    frameTime = newTime;
  }

protected:
  PointManager pm;
  MouseToTrackerRef _mouseToTracker;
  CoordinateFrame _virtualToRoomSpace;
  CoordinateFrame _owm;
  CoordinateFrame _lastTrackerLocation;
  Table<std::string, CoordinateFrame> _trackerFrames;
  bool _moving;
  clock_t frameTime;
  AnimationController ac;
};

int main(int argc, char **argv )
{
  std::string setup;
  std::string dataFile;
  if (argc >= 2)
  {
    setup = std::string(argv[1]);
  }
  if(argc >= 3)
  {
    dataFile = std::string(argv[2]);
  }
  else{
    dataFile = "/users/jtveite/data/jtveite/slices-61.out";
  }


  DinoApp *app = new DinoApp(setup, dataFile);
  app->run();
  return 0;

}
