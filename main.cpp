#include <vrg3d/VRG3D.h>
#include "pointmanager.h"
#include "animationcontroller.h"
#include "filter.h"

#define PROFILING
#undef PROFILING

#ifdef PROFILING
#include "gperftools/profiler.h"
#endif

using namespace G3D;

class DinoApp : public VRApp
{
public:
  DinoApp(std::string setup, std::string dataFile, std::string pathFile, bool showAllPaths = false) : VRApp(),
  ac()
  {
  
    std::string profFile = "/users/jtveite/d/prof/profile-" + setup;
#ifdef PROFILING
    ProfilerStart(profFile.c_str());
#endif


    Log *dinolog = new Log("dino-log.txt");
    init(setup, dinolog);
    _mouseToTracker = new MouseToTracker(getCamera(), 2);
    frameTime = clock();

//    pm.ReadFile("/users/jtveite/data/jtveite/slices-130.out");
//    pm.ReadFile("/users/jtveite/dinotrackviewer/test-data");
    pm.ReadFile(dataFile, true);
    pm.SetupDraw(showAllPaths);
    if (pathFile != "" && pathFile != "a"){
      pm.ReadPathlines(pathFile);
    }
    Matrix3 scale = Matrix3::fromDiagonal(Vector3(20, 20, 20));
    CoordinateFrame scaleC (scale);
    CoordinateFrame rotate = CoordinateFrame::fromXYZYPRDegrees(
      2, 0, -6,
      -90, -90, 180);
    _owm = rotate * scale;
    ac.setFrameCount(pm.getLength());
    ac.setSpeed(15);

    //std::cout << GL_NO_ERROR << ' ' << GL_INVALID_ENUM << ' ' << GL_INVALID_VALUE << ' ' << GL_INVALID_OPERATION << ' ' << GL_INVALID_FRAMEBUFFER_OPERATION << std::endl;
    //
    targetTracker = "Wand_Tracker";
    if (setup == "" || setup == "desktop"){
      targetTracker = "Mouse1_Tracker";
    }

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
      else if (eventName == "Wand_Left_Btn_down"){
        _placePathline = true; 
      }
      else if (eventName == "kbd_Q_down"){
        _placePathline = true;
        printf("QQQQQQQ\n");
      }
      else if (eventName == "kbd_DOWN_down"){
        ac.decreaseSpeed();
      }
      else if (eventName == "kbd_UP_down"){
        ac.increaseSpeed();
      }
      else if (eventName == "B04_down"){
       // ac.decreaseSpeed();
       pm.pointSize /= 1.3;
      }
      else if (eventName == "B03_down"){
        //ac.increaseSpeed();
        pm.pointSize *= 1.3;
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
      else if (eventName == "kbd_SPACE_down"){
        ac.togglePlay();
      }
      else if (eventName == "kbd_LEFT_down"){
        ac.stepBackward();
      }
      else if (eventName == "kbd_RIGHT_down"){
        ac.stepForward();
      }
      

      else if (eventName == "B09_down"){
        pm.SetFilter(new Filter());
      }
      else if (eventName == "B10_down"){
        pm.SetFilter(new MotionFilter(motionThreshold));
      }
      else if (eventName == "B11_down" || eventName == "kbd_J_down"){
        motionThreshold *= 1.414;
        pm.SetFilter(new MotionFilter(motionThreshold));
      }
      else if (eventName == "B12_down" || eventName == "kbd_K_down"){
        motionThreshold /= 1.414;
        pm.SetFilter(new MotionFilter(motionThreshold));
      }



      else if (beginsWith(eventName, "aimo")){}
      else if (eventName == "SynchedTime"){}
      else{
        //std::cout << eventName << std::endl;
      }



    }
    
  }
 

  void doGraphics(RenderDevice *rd)
  {
    _lastFrame = ac.getFrame();
    float t = _lastFrame;
    //std::cout << "Rendering frame: " << _frameCounter << std::endl;

    //Do rotation of trackers if needed
    //std::string targetTracker = "Mouse1_Tracker";
    //std::string targetTracker = "Wand_Tracker";
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
    if (_placePathline){
      _placePathline = false;
    //  CoordinateFrame oldSpace = _lastTrackerLocation * _owm.inverse();
     // Vector3 location = oldSpace.translation;
      Vector3 location = _lastTrackerLocation.translation;
      //location = _lastTrackerLocation.pointToWorldSpace(Vector3(0,0,0));
      Matrix4 owm = _owm.toMatrix4();
      Vector4 l = owm.inverse() * Vector4(location, 1.0);
      pm.AddPathline(l.xyz(), t);
      //std::cout << l.xyz() << std::endl;
      //printf("placing a pathline at %f, %f, %f\n", location.x, location.y, location.z);
      //printf("placing a pathline at %f, %f, %f\n", location.x, location.y, location.z);
    }

    rd->pushState();
    //rd->setObjectToWorldMatrix(_virtualToRoomSpace);
    int gl_error;
    while((gl_error = glGetError()) != GL_NO_ERROR)
    {

      std::cout << "Flushing gl errors " << gl_error << std::endl;
    }
   
    rd->pushState();

    rd->setObjectToWorldMatrix(_owm);
    Matrix4 mvp = rd->invertYMatrix() * rd->modelViewProjectionMatrix();
    
    //std::cout << t << std::endl;
    pm.Draw(rd, t, mvp);
    rd->popState();

    Vector3 hp = getCamera()->getHeadPos();
   // printf("Head position: %f, %f, %f\n", hp.x, hp.y, hp.z);
    Vector3 lv = getCamera()->getLookVec();
 
    //Draw a small ball at the tracker location.
    //
    rd->pushState();
    rd->setObjectToWorldMatrix(CoordinateFrame());
    Sphere s (_lastTrackerLocation.translation, 0.02);
    Draw::sphere(s, rd, Color3::red());
    rd->popState();

 //  printf("Look Vector: %f, %f, %f\n", lv.x, lv.y, lv.z);

    rd->popState();
    clock_t newTime = clock();
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
  int _lastFrame;
  bool _placePathline;
  std::string targetTracker;
  float motionThreshold = 0.01;
};

int main(int argc, char **argv )
{
  std::string setup;
  std::string dataFile;
  std::string pathsFile;
  bool showAllPaths = false;
  if (argc >= 2)
  {
    setup = std::string(argv[1]);
  }
  if(argc >= 3)
  {
    dataFile = std::string(argv[2]);
  }
  else{
    dataFile = "/users/jtveite/data/jtveite/slices-68.out";
  }
  if (argc >= 4)
  {
    pathsFile = std::string(argv[3]);
  }
  else{
    pathsFile = "";
  }
  if (argc >= 5)
  {
    showAllPaths = true;
  }
    
  DinoApp *app = new DinoApp(setup, dataFile, pathsFile,  showAllPaths);
  app->run();
  return 0;

}
