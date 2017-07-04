#include <string>
#include <iostream>
#include <math.h>

#include <display/VRConsoleNode.h>
#include <main/VRMain.h>
#include <main/VREventHandler.h>
#include <main/VRRenderHandler.h>
#include <math/VRMath.h>


#if defined(WIN32)
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#include <gl/GLU.h>
#elif defined(__APPLE__)
#include <GL/glew.h>
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "slide.h"
#include "vrpoint.h"
#include "vertex.h"
#include "filter.h"
#include "pointmanager.h"
#include "animationcontroller.h"
#include "footmeshviewer.h"
#include "webupdatereader.h"
#include "PathAlignmentSimilarityEvaluator.h"
//#include "GLUtil.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/type_ptr.hpp"


using namespace MinVR;


void printMatrix4(VRMatrix4 m){
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      printf("%6.2f ", m[i][j]);
    }
    printf("\n");
  }
}

void printMat4(glm::mat4 m){
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      printf("%6.2f ", m[j][i]);
    }
    printf("\n");
  }
}

enum struct Mode {STANDARD, ANIMATION, FILTER, SLICES, PREDICT, CLUSTER, PATHSIZE, SIMILARITY};

class MyVRApp : public VREventHandler, public VRRenderHandler, public UpdateChecker {
public:
  MyVRApp(int argc, char** argv) : _vrMain(NULL), _quit(false), first(true) {
		_vrMain = new VRMain();
        _vrMain->initialize(argc, argv);
      	_vrMain->addEventHandler(this);
		_vrMain->addRenderHandler(this);

    const unsigned char* s = glGetString(GL_VERSION);
    printf("GL Version %s.\n", s);
        _horizAngle = 0.0;
        _vertAngle = 0.0;
		_radius =  4.0;
        _incAngle = -0.1f;
    _pm = new PointManager();
    //_pm->ReadFile("data/slices-68-trimmed.out");
    _pm->ReadFile("active-dataset.out");
    printf("Loaded file with %d timesteps.\n", _pm->getLength());
    std::cout << _pm->getLength() << std::endl;
    //_pm->ReadMoVMFs("paths.movm");
    _pm->ReadClusters("active.clusters");
    _pm->ReadPathlines("active.pathlines");
    _pm->colorByCluster = true;
    _pm->simEval = new PathAlignmentSimilarityEvaluator();
    ac.setFrameCount(_pm->getLength());
    ac.setSpeed(15);
    mode = Mode::STANDARD;
    _slicer = new SliceFilter();
    _wur = new WebUpdateReader("weblog", this);
	}

  void HandleUpdate(std::string key, std::string value){
    std::cout << "Got update from web    " << key << " : " << value << std::endl;
    bool trueval = false;
    if (value == "1"){ trueval = true; }
    else if (value == "0") { trueval = false;}
    //TODO: FIX TO HANDLE NOT JUST BOOLS 
   
    //TODO: Figure out a good structure for this.
    if (key == "ResetPaths"){
      _pm->ClearPathlines();
      _pm->ResetPrediction();
      _pm->clustering=false;
      _pm->currentCluster = -1;
      _pm->drawAllClusters = false;
      _pm->pathlineMin = 0.0;
      _pm->pathlineMax = 1.0;
    }

    if (key == "Clustering"){
      mode = Mode::CLUSTER;
      _pm->clustering = trueval;
    }
    
    if (key == "ClusterByCluster"){
      mode = Mode::CLUSTER;
      iterateClusters = trueval;
    }

    if (key == "DrawAllClusters"){
      _pm->drawAllClusters = trueval;
    }

    if (key == "ShowFoot"){
      showFoot = trueval;
    }

    if (key == "Pathlines--Poisson"){
      _pm->ClearPathlines();
      _pm->ReadPathlines("study_paths/poisson");
    }
    
    if (key == "Pathlines--PCA"){
      _pm->ClearPathlines();
      _pm->ReadPathlines("study_paths/pca");
    }
    
    if (key == "Pathlines--Signatures"){
      _pm->ClearPathlines();
      _pm->ReadPathlines("study_paths/signatures");
    }
    
    if (key == "Pathlines--SCPT"){
      _pm->ClearPathlines();
      _pm->ReadPathlines("study_paths/scpt");
    }
  } 

	virtual ~MyVRApp() {
		_vrMain->shutdown();
		delete _vrMain;
	}

	// Callback for event handling, inherited from VREventHandler
	virtual void onVREvent(const std::string &eventName, VRDataIndex *eventData) {
    if (eventName != "/aimon_13_Change" &&
        eventName != "/aimon_14_Change" &&eventName != "/aimon_15_Change" && eventName != "/aimon_16_Change"){

		std::cout << "Event: " << eventName << std::endl;
    }

    if (eventName == "/Wand_Move"){
      VRMatrix4 wandPosition = eventData->getValue("/Wand_Move/Transform");
      glm::mat4 wandPos = glm::make_mat4(wandPosition.m);
      //printMat4(wandPos);
      if(_moving){
        _owm = wandPos / _lastWandPos * _owm;
      }
      if (_movingSlide){
        _slideMat = wandPos / _lastWandPos * _slideMat;
      }
      _lastWandPos = wandPos;
    }

    if (eventName == "/KbdA_Down"){
      std::cout << "Test" << std::endl;
      std::cout << eventData->printStructure() << std::endl;
    }


    if (eventName == "/Kbd1_Down" || eventName == "/Mouse_Up_Down"){
      mode = Mode::STANDARD;
      printf("moving to standard\n");
    }
    else if (eventName == "/Kbd2_Down" || eventName == "/Mouse_Down_Down"){
      //mode = Mode::CLUSTER;
      //mode = Mode::ANIMATION;
      //mode = Mode::PATHSIZE;
      _pm->colorByCluster = ! _pm->colorByCluster;
      std::cout << _pm->colorByCluster << std::endl;
    }
    else if (eventName == "/Kbd3_Down" || eventName == "/Mouse_Left_Down"){
      //mode = Mode::FILTER;
      //_pm->SetFilter(new MotionFilter(motionThreshold));
      _pm->ClearPathlines();
      _pm->ResetPrediction();
      _pm->clustering = false;
      _pm->currentCluster = -1;
      _pm->colorBySimilarity = false;
    }
    else if (eventName == "/Kbd4_Down" || eventName == "/Mouse_Right_Down"){
      mode = Mode::SIMILARITY;
      //_pm->clustering = true;
      //_pm->ClearPathlines();
      //mode = Mode::SLICES;
      //_pm->SetFilter(_slicer);
    }
    else if (eventName == "/Kbd5_Down"){//instacrashes :( || eventName == "/Mouse_Left_Click_Down"){
      _pm->SetShaders();

    }
    else if (eventName == "/Kbd6_Down" || eventName == "/Mouse_Right_Click_Down"){
      _pm->colorPathsBySimilarity = !_pm->colorPathsBySimilarity;
    }

    else if (eventName == "/MouseBtnLeft_Down" || eventName == "/Wand_Bottom_Trigger_Down"){
      _moving = true;
    }
    else if (eventName == "/MouseBtnLeft_Up" || eventName == "/Wand_Bottom_Trigger_Up"){
      _moving = false;
    }
    else if (eventName == "/Wand_Joystick_Press_Down"){
      _movingSlide = true;
    }
    else if (eventName == "/Wand_Joystick_Press_Up"){
      _movingSlide = false;
    }
    else if (eventName == "/KbdQ_Down" || eventName == "/Wand_Trigger_Top_Change"){
      _placePathline = true;
      }
         else if (eventName == "/KbdDown_Down" || eventName == "/Wand_Down_Down"){
      if (mode == Mode::STANDARD){
        _pm->pointSize /= 1.3;
      }
      if (mode == Mode::ANIMATION){
        ac.decreaseSpeed();
      }
      if (mode == Mode::SLICES){
        _slicer->addStart(.001);
        _pm->SetFilter(_slicer);
      }
      if (mode == Mode::FILTER){
        motionThreshold *= 1.414;
        _pm->SetFilter(new MotionFilter(motionThreshold));
      }
      if (mode == Mode::PREDICT){
        _placeClusterSeed = false;
      }
      if (mode == Mode::CLUSTER && iterateClusters){
        _pm->currentCluster--;
        if (_pm->currentCluster < 0){
          _pm->currentCluster = _pm->clusters.size() - 1;
        }
        _pm->clustersChanged = true;
      }
      if (mode == Mode::PATHSIZE){
        _pm->pathlineMin -= 0.04; 
      }
      if (mode == Mode::SIMILARITY){
        _similarityCount /= 1.2;
        _pm->ExpandClosestPoints(_similarityCount);
      }
    }
    else if (eventName == "/KbdUp_Down" || eventName == "/Wand_Up_Down"){
      if (mode == Mode::STANDARD){
        _pm->pointSize *= 1.3;
      }
      if (mode == Mode::ANIMATION){
        ac.increaseSpeed();
      }
      if (mode == Mode::SLICES){
        _slicer->addStart(-.001);
        _pm->SetFilter(_slicer);
      }
      if (mode == Mode::FILTER){
        motionThreshold /= 1.414;
        _pm->SetFilter(new MotionFilter(motionThreshold));
      }
      if (mode == Mode::PREDICT){
        //_pm->SearchForSeeds();
        _placeClusterSeed = true;
      }
      if (mode == Mode::CLUSTER && iterateClusters){
        _pm->currentCluster++;
        if (_pm->currentCluster >= _pm->clusters.size()){
          _pm->currentCluster = 0;
        }
        _pm->clustersChanged = true;
      }
      if (mode == Mode::PATHSIZE){
        _pm->pathlineMin += 0.04; 
      }
      if (mode == Mode::SIMILARITY){
        _similarityCount *= 1.2;
        _pm->ExpandClosestPoints(_similarityCount);
      }
    }
    else if (eventName == "/KbdLeft_Down" || eventName == "/Wand_Left_Down"){
      if (mode == Mode::SLICES){
        _slicer->addGap(-0.0025);
        _pm->SetFilter(_slicer);
      }
      else if (mode == Mode::PREDICT){
        //_pm->SearchForSeeds();
      }
      else if (mode == Mode::PATHSIZE){
        _pm->pathlineMax -= 0.04; 
      }
      else if (mode == Mode::SIMILARITY){
        _similarityGo = true;
      }
      else{
        ac.stepBackward();
      }
    }
    else if (eventName == "/KbdRight_Down" || eventName == "/Wand_Right_Down"){
      if (mode == Mode::SLICES){
        _slicer->addGap(.0025);
        _pm->SetFilter(_slicer);
      }
      else if (mode == Mode::PATHSIZE){
        _pm->pathlineMax += 0.04; 
      }
      else{
        ac.stepForward();
      }
    }
    else if (eventName == "/KbdSpace_Down" || eventName == "/Wand_Select_Down"){
      ac.togglePlay();
      if (mode == Mode::PREDICT){
        //_pm->SearchForSeeds();
      }
    }
    else if (eventName == "/KbdU_Down"){
      _pm->pathlineMin += 0.05;
    }
    else if (eventName == "/KbdJ_Down"){
      _pm->pathlineMin -= 0.05;
    }
    else if (eventName == "/KbdI_Down"){
      _pm->pathlineMax += 0.05;
    }
    else if (eventName == "/KbdK_Down"){
      _pm->pathlineMax -= 0.05;
    }


		if (eventName == "/KbdEsc_Down") {
			_quit = true;
		}
        else if (eventName == "/MouseBtnLeft_Down") {
          _radius += 5.0 * _incAngle;
        }
        else if (eventName == "/MouseBtnRight_Down") {
          _radius -= 5.0 * _incAngle;
        }
        else if ((eventName == "/KbdLeft_Down") || (eventName == "/KbdLeft_Repeat")) {
          //_horizAngle -= _incAngle;
        }
        else if ((eventName == "/KbdRight_Down") || (eventName == "/KbdRight_Repeat")) {
          //_horizAngle += _incAngle;
        }
        else if ((eventName == "/KbdUp_Down") || (eventName == "/KbdUp_Repeat")) {
          //_vertAngle -= _incAngle;
        }
        else if ((eventName == "/KbdDown_Down") || (eventName == "/KbdDown_Repeat")) {
          //_vertAngle += _incAngle;
        }
      
        if (_horizAngle > 6.283185) _horizAngle -= 6.283185;
        if (_horizAngle < 0.0) _horizAngle += 6.283185;
      
        if (_vertAngle > 6.283185) _vertAngle -= 6.283185;
        if (_vertAngle < 0.0) _vertAngle += 6.283185;
	}

  
    virtual void onVRRenderContext(VRDataIndex *renderState, VRDisplayNode *callingNode) {
        if (!renderState->exists("IsConsole", "/")) {
        }
        _wur->checkForUpdates();
        time = ac.getFrame();
    }

	int count;
  
	// Callback for rendering, inherited from VRRenderHandler
	virtual void onVRRenderScene(VRDataIndex *renderState, VRDisplayNode *callingNode) {
      glCheckError();
		if (renderState->exists("IsConsole", "/")) {
			VRConsoleNode *console = dynamic_cast<VRConsoleNode*>(callingNode);
			console->println("Console output...");
		}
		else {

    if (first){
      glewExperimental = GL_TRUE;
      glewInit();
      _pm->SetupDraw();
      _slide.Initialize("slide.png", glm::vec3(3,-3,0), glm::vec3(0,2,0), glm::vec3(0,0,1.544));
      _fmv.ReadFiles("feet.feet");
      first = false;
    }
    GLuint test;
      glCheckError();
    glGenBuffers(1, &test);
      glCheckError();
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glClearDepth(1.0f);
			count++;
			glClearColor(0.0, 0.0, 0.0, 1.f);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			//glClear(GL_DEPTH_BUFFER_BIT);
      glCheckError();

      glm::mat4 M, V, P;
      glm::mat4 MVP;
      if (renderState->exists("ProjectionMatrix", "/")) {
          // This is the typical case where the MinVR DisplayGraph contains
          // an OffAxisProjectionNode or similar node, which sets the
          // ProjectionMatrix and ViewMatrix based on head tracking data.
          VRMatrix4 PP = renderState->getValue("ProjectionMatrix", "/");
          
          // In the original adaptee.cpp program, keyboard and mouse commands
          // were used to adjust the camera.  Now that we are creating a VR
          // program, we want the camera (Projection and View matrices) to
          // be controlled by head tracking.  So, we switch to having the
          // keyboard and mouse control the Model matrix.  Guideline: In VR,
          // put all of the "scene navigation" into the Model matrix and
          // leave the Projection and View matrices for head tracking.
          VRMatrix4 MM = VRMatrix4::translation(VRVector3(0.0, 0.0, -_radius)) *
                        VRMatrix4::rotationX(_vertAngle) *
                        VRMatrix4::rotationY(_horizAngle);
        
          VRMatrix4 VV= renderState->getValue("ViewMatrix", "/");
          //glLoadMatrixd((VV*MM).m);
          V = glm::make_mat4(VV.m);
          P = glm::make_mat4(PP.m);
          M = glm::make_mat4(MM.m);
      }
      else {
          // If the DisplayGraph does not contain a node that sets the
          // ProjectionMatrix and ViewMatrix, then we must be in a non-headtracked
          // simple desktop mode.  We can just set the projection and modelview
          // matrices the same way as in adaptee.cpp.
                        
          
          P = glm::perspective(1.6f*45.f, 1.f, 0.1f, 100.0f);

          glm::vec3 pos = glm::vec3(_radius * cos(_horizAngle) * cos(_vertAngle),
                                    -_radius * sin(_vertAngle),
                                    _radius * sin(_horizAngle) * cos(_vertAngle));
          glm::vec3 up = glm::vec3( cos(_horizAngle) * sin(_vertAngle),
                                    cos(_vertAngle),
                                    sin(_horizAngle) * sin(_vertAngle));

          V = glm::lookAt(pos, glm::vec3(0.f), up);
         
       }

        glm::mat4 scale = glm::scale(glm::mat4(), glm::vec3(40,40,40));
        glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3(1, -1, 0));
         

        //in desktop mode, +x is away from camera, +z is right, +y is up 
        //M = translate * scale;
        glm::mat4 slideM = _slideMat * M;
        M = _owm * M * translate * scale;
        //V = glm::transpose(V);
        MVP = P * V * M;
        glm::mat4 t = V ;
        
         
        glm::vec3 location = glm::vec3(_lastWandPos[3]);
        glm::vec4 modelPos = glm::inverse(M) * glm::vec4(location, 1.0);
        _pm->TempPathline(glm::vec3(modelPos), time);
            /*printf("World space location: %f, %f, %f\n", location.x, location.y, location.z);
            printf("Model space location: %f, %f, %f\n", modelPos.x, modelPos.y, modelPos.z);
            printMat4(glm::inverse(M));
            printMat4(M);*/
        if (_placePathline){
            _placePathline = false;
            if (mode == Mode::PREDICT){
              _pm->ShowCluster(glm::vec3(modelPos), time);
            }
            else{
              _pm->AddPathline(glm::vec3(modelPos), time);
            }
        }
        if (_placeClusterSeed){
          _pm->ShowCluster(glm::vec3(modelPos), time);
        }
        if (_similarityGo){
          _similarityGo = false;
          _pm->FindClosestPoints(glm::vec3(modelPos), time, (int) _similarityCount);
        }
        
        
        
        glCheckError();
        _pm->Draw(time, MVP );
        if (showFoot){
          _fmv.Draw(time, MVP);
        }
        glCheckError();
        //_slide.Draw(P * V * slideM);
        glCheckError();
		}
	}

	void run() {
		while (!_quit) {
      glCheckError();
			_vrMain->mainloop();
		}
	}

protected:
	VRMain *_vrMain;
	bool _quit;
    double _horizAngle, _vertAngle, _radius, _incAngle;
  PointManager* _pm;
  bool first;
  int frame;
  Mode mode;
  float motionThreshold = 0.01;
  AnimationController ac;
  bool _moving = false;
  bool _placePathline;
  bool _placeClusterSeed = false;
  SliceFilter* _slicer;
  glm::mat4 _owm;
  glm::mat4 _lastWandPos;
  FootMeshViewer _fmv;
  bool showFoot = true;
  WebUpdateReader* _wur;
  bool iterateClusters = false;
  float time;
  Slide _slide;
  glm::mat4 _slideMat;
  bool _movingSlide = false;
  double _similarityCount = 50;
  bool _similarityGo = false;
};



int main(int argc, char **argv) {
    glm::vec3 a (1, 2, 3);
    glm::vec4 b (a, 1.0);
    glm::vec3 c (b);
    printf("%f,%f,%f\n", a.x, a.y, a.z);
    printf("%f, %f, %f, %f\n", b.x, b.y, b.z, b.w);
    printf("%f, %f, %f\n", c.x, c.y, c.z);
    MyVRApp app(argc, argv);
  	app.run();

    return 0;
}

