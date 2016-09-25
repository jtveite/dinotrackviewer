#include "pointmanager.h"
#include "animationcontroller.h"
#include "filter.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <unordered_map>

//MinVR includes
#include <display/VRConsoleNode.h>
#include <main/VRMain.h>
#include <main/VREventHandler.h>
#include <main/VRRenderHandler.h>
#include <math/VRMath.h>

#define PROFILING
#undef PROFILING

#ifdef PROFILING
#include "gperftools/profiler.h"
#endif

#include <GL/gl.h>


enum struct Mode { STANDARD, ANIMATION, FILTER, SLICES};

glm::mat4 VRMtoGLM(VRMatrix4 vrm){
  return glm::make_mat4(vrm.m);
}


class DinoApp : public MinVR::VREventHandler, public MinVR::VRRenderHandler
{
public:
  DinoApp(std::string setup, std::string dataFile, std::string pathFile, int argc, char**argv,  bool showAllPaths = false) : _vrMain(NULL)
  {
  
    std::string profFile = "/users/jtveite/d/prof/profile-" + setup;
#ifdef PROFILING
    ProfilerStart(profFile.c_str());
#endif

    _vrMain = new MinVR::VRMain();
    _vrMain->addEventHandler(this);
    _vrMain->addRenderHandler(this);
    _vrMain->initialize(argc, argv);
    //_mouseToTracker = new MouseToTracker(getCamera(), 2);
    frameTime = clock();

//    pm.ReadFile("/users/jtveite/data/jtveite/slices-130.out");
//    pm.ReadFile("/users/jtveite/dinotrackviewer/test-data");
    pm.ReadFile(dataFile, true);
    
   /* 
    int maj,min;
    glGetIntegerv(GL_MAJOR_VERSION, &maj);
    glGetIntegerv(GL_MINOR_VERSION, &min);
    const unsigned char* s = glGetString(GL_VERSION);
    printf("GL Version %d.%d    %d\n", maj, min, GL_MAJOR_VERSION);
    printf("alternate version %s.\n", s);
    std::cout << std::endl;
*/

 //   pm.SetupDraw(showAllPaths);
    if (pathFile != "" && pathFile != "a"){
      pm.ReadPathlines(pathFile);
    }
    glm::mat4 scale = glm::scale(glm::mat4(), glm::vec3(20,20,20));
    glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3(2, 0, -6));
    glm::mat4 rotate = glm::eulerAngleYXZ(-90, -90, 180);
    _owm = translate * rotate * scale;
    ac.setFrameCount(pm.getLength());
    ac.setSpeed(15);

    //std::cout << GL_NO_ERROR << ' ' << GL_INVALID_ENUM << ' ' << GL_INVALID_VALUE << ' ' << GL_INVALID_OPERATION << ' ' << GL_INVALID_FRAMEBUFFER_OPERATION << std::endl;
    //
    targetTracker = "Wand_Tracker";
    if (setup == "" || setup == "desktop"){
      targetTracker = "Mouse1_Tracker";
    }

    _slicer = new SliceFilter();
  }
  virtual ~DinoApp() {
    _vrMain->shutdown();
    delete _vrMain;
#ifdef PROFILING
    ProfilerFlush();
    ProfilerStop();
#endif
  }

  //general skeleton of user input from demo app
  virtual void onVREvent(const std::string &eventName, VRDataIndex *eventData)
  {
      /*
      if (endsWith(eventName, "_Tracker")) {
        auto search = _trackerFrames.find(eventName);//ex from cppreference.com
        //Probably don't need to search since implementation seems to do it?
        if (search != _trackerFrames.end())){
          _trackerFrames[eventName] = events[i]->getglm::mat4Data(); 
        }
        else {
          _trackerFrames[eventName] = events[i]->getglm::mat4Data());
        }
      }*/
      if (eventName == "B09_down" || eventName == "kbd_1_down"){
        mode = Mode::STANDARD;
      }
      else if (eventName == "B10_down" || eventName == "kbd_2_down"){
        mode = Mode::ANIMATION;
      }
      else if (eventName == "B11_down" || eventName == "kbd_3_down"){
        mode = Mode::FILTER;
        pm.SetFilter(new MotionFilter(motionThreshold));
      }
      else if (eventName == "B12_down" || eventName == "kbd_4_down"){
        mode = Mode::SLICES;
        pm.SetFilter(_slicer);
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
        if (mode == Mode::STANDARD){
          pm.pointSize /= 1.3;
        }
        if (mode == Mode::ANIMATION){
          ac.decreaseSpeed();
        }
        if (mode == Mode::SLICES){
          _slicer->addStart(.001);
          pm.SetFilter(_slicer);
        }
        if (mode == Mode::FILTER){
          motionThreshold *= 1.414;
          pm.SetFilter(new MotionFilter(motionThreshold));
        }
      }
      else if (eventName == "B03_down"){
        if (mode == Mode::STANDARD){
          pm.pointSize *= 1.3;
        }
        if (mode == Mode::ANIMATION){
          ac.increaseSpeed();
        }
        if (mode == Mode::SLICES){
          _slicer->addStart(-.001);
          pm.SetFilter(_slicer);
        }
        if (mode == Mode::FILTER){
          motionThreshold /= 1.414;
          pm.SetFilter(new MotionFilter(motionThreshold));
        }
      }
      
      else if (eventName == "B05_down"){
        if (mode == Mode::STANDARD || mode == Mode::ANIMATION){
          ac.stepBackward();
        }
        if (mode == Mode::SLICES){
          _slicer->addGap(-.0025);
          pm.SetFilter(_slicer);
        }
      }
      else if (eventName == "B06_down"){
        if (mode == Mode::STANDARD || mode == Mode::ANIMATION){
          ac.stepForward();
        }
        if (mode == Mode::SLICES){
          _slicer->addGap(.0025);
          pm.SetFilter(_slicer);
        }
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
      else if (eventName == "kbd_C_down"){
        pm.pointSize *= 1.3;
      }
      else if (eventName == "kbd_V_down"){
        pm.pointSize /= 1.3;
      }
      
/*
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
*/
      else if (eventName == "kbd_T_down"){
        pm.SetFilter(_slicer);
      }

      else if (eventName == "kbd_R_down"){
        _slicer->addStart(0.0025);
        pm.SetFilter(_slicer);
      }

      else if (eventName == "kbd_F_down"){
        _slicer->addGap(0.0025);
        pm.SetFilter(_slicer);
      }

      else if (eventName == "kbd_G_down"){
        _slicer->addSize(0.0025);
        pm.SetFilter(_slicer);
      }

      //else if (beginsWith(eventName, "aimo")){}
      //else if (eventName == "SynchedTime"){}
      else{
        std::cout << eventName << std::endl;
      }



    
  }
 

  void onVRRenderScene(VRDataIndex *renderState, MinVR::VRDisplayNode *callingNode)
  { 
    if (renderState->exists("IsConsole", "/")) {
      MinVR::VRConsoleNode * console = dynamic_cast<MinVR::VRConsoleNode*>(callingNode);
      console->println("console output...");
      printf("console");
      return;
    }
    if (needsSetup){
      needsSetup = false;

      const unsigned char* s = glGetString(GL_VERSION);
      printf("glv %s\n", s);
      glewInit();
      GLuint test;
      glGenBuffers(1, &test);

      pm.SetupDraw();

    }
    _lastFrame = ac.getFrame();
    float t = _lastFrame;
    //std::cout << "Rendering frame: " << _frameCounter << std::endl;

    //Do rotation of trackers if needed
    //std::string targetTracker = "Mouse1_Tracker";
    //std::string targetTracker = "Wand_Tracker";
    for (auto kv : _trackerFrames){
      std::string trackerName = kv.first;
      glm::mat4 trackerFrame = kv.second;
      if (trackerName == targetTracker){
        if (_moving){
          glm::mat4 amountMoved = trackerFrame * glm::inverse( _lastTrackerLocation);
          _owm = amountMoved * _owm;
        }
        _lastTrackerLocation = trackerFrame;
        
      }

    }
    if (_placePathline){
      _placePathline = false;
    //  glm::mat4 oldSpace = _lastTrackerLocation * _owm.inverse();
     // Vector3 location = oldSpace.translation;
      //glm::vec3 location = _lastTrackerLocation.translation;
      //location = _lastTrackerLocation.pointToWorldSpace(Vector3(0,0,0));
      //Matrix4 owm = _owm.toMatrix4();
      //Vector4 l = owm.inverse() * Vector4(location, 1.0);
      //glm::vec3 ll (l.x, l.y, l.z);
      //pm.AddPathline(ll, t);
      //std::cout << l.xyz() << std::endl;
      //printf("placing a pathline at %f, %f, %f\n", location.x, location.y, location.z);
      //printf("placing a pathline at %f, %f, %f\n", location.x, location.y, location.z);
    }
      /*
      Vector3 location = _lastTrackerLocation.translation;
      //location = _lastTrackerLocation.pointToWorldSpace(Vector3(0,0,0));
      Matrix4 owm = _owm.toMatrix4();
      Vector4 l = owm.inverse() * Vector4(location, 1.0);
      glm::vec3 ll (l.x, l.y, l.z);
      //pm.TempPathline(ll, t);
      */

    //rd->setObjectToWorldMatrix(_virtualToRoomSpace);
    int gl_error;
    while((gl_error = glGetError()) != GL_NO_ERROR)
    {

      std::cout << "Flushing gl errors " << gl_error << std::endl;
    }
    
    glm::mat4 P;
    glm::mat4 V;
    if (renderState->exists("ProjectionMatrix", "/")){
      P = VRMtoGLM(renderState->getValue("ProjectionMatrix", "/"));
      V = VRMtoGLM(renderState->getValue("ViewMatrix", "/"));
    }
    float *hack;
    hack = glm::value_ptr(_owm);
    glm::mat4 M = glm::make_mat4(hack);

    
    for(int i = 0; i < 4 ; i++){
      for(int j = 0; j < 4; j++){
       // printf("%3f  ", P[i][j]); 
      }
     // printf("\n");
    }

    glm::mat4 mvp = P * V * M;


    
    //std::cout << t << std::endl;
    pm.Draw(t, mvp);

 
    //Draw a small ball at the tracker location.
    //
    //Sphere s (_lastTrackerLocation.translation, 0.02);
    //Draw::sphere(s, rd, Color3::red());

 //  printf("Look Vector: %f, %f, %f\n", lv.x, lv.y, lv.z);

    clock_t newTime = clock();
    frameTime = newTime;
  }

  void run(){
    while(true) {
      _vrMain->mainloop();
    }
  }

protected:
  bool needsSetup = true;
  MinVR::VRMain *_vrMain;
  PointManager pm;
  glm::mat4 _virtualToRoomSpace;
  glm::mat4 _owm;
  glm::mat4 _lastTrackerLocation;
  std::unordered_map<std::string, glm::mat4> _trackerFrames;
  bool _moving;
  clock_t frameTime;
  AnimationController ac;
  int _lastFrame;
  bool _placePathline;
  std::string targetTracker;
  float motionThreshold = 0.01;
  SliceFilter* _slicer;
  Mode mode = Mode::STANDARD;
};

int main(int argc, char **argv )
{
  std::string setup = "desktop.xml";
  std::string dataFile;
  std::string pathsFile;
  bool showAllPaths = false;
  if (argc >= 2)
  {
    setup = std::string(argv[1]);
  }
  if(argc >= 3)
  {
    //dataFile = std::string(argv[2]);
  }
  else{
    dataFile = "/users/jtveite/data/jtveite/slices-68.out";
  }
    dataFile = "/users/jtveite/data/jtveite/slices-68.out";
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
    
  DinoApp *app = new DinoApp(setup, dataFile, pathsFile, argc, argv, showAllPaths);
  app->run();
  return 0;

}
