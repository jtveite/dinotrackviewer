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
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif


#include "vrpoint.h"
#include "vertex.h"
#include "filter.h"
#include "pointmanager.h"
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

class MyVRApp : public VREventHandler, public VRRenderHandler {
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
		_radius =  10.0;
        _incAngle = -0.1f;
    _pm = new PointManager();
    _pm->ReadFile("data/slices-68-trimmed.out");
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
          _horizAngle -= _incAngle;
        }
        else if ((eventName == "/KbdRight_Down") || (eventName == "/KbdRight_Repeat")) {
          _horizAngle += _incAngle;
        }
        else if ((eventName == "/KbdUp_Down") || (eventName == "/KbdUp_Repeat")) {
          _vertAngle -= _incAngle;
        }
        else if ((eventName == "/KbdDown_Down") || (eventName == "/KbdDown_Repeat")) {
          _vertAngle += _incAngle;
        }
      
        if (_horizAngle > 6.283185) _horizAngle -= 6.283185;
        if (_horizAngle < 0.0) _horizAngle += 6.283185;
      
        if (_vertAngle > 6.283185) _vertAngle -= 6.283185;
        if (_vertAngle < 0.0) _vertAngle += 6.283185;
	}

  
    virtual void onVRRenderContext(VRDataIndex *renderState, VRDisplayNode *callingNode) {
        if (!renderState->exists("IsConsole", "/")) {
        }
    }

	int count;
  
	// Callback for rendering, inherited from VRRenderHandler
	virtual void onVRRenderScene(VRDataIndex *renderState, VRDisplayNode *callingNode) {
		if (renderState->exists("IsConsole", "/")) {
			VRConsoleNode *console = dynamic_cast<VRConsoleNode*>(callingNode);
			console->println("Console output...");
		}
		else {

    if (first){
      glewExperimental = GL_TRUE;
      glewInit();
      _pm->SetupDraw();
      first = false;
    }
    GLuint test;
    glGenBuffers(1, &test);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glClearDepth(1.0f);
			count++;
			glClearColor(0.0, 0.0, 0.0, 1.f);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			//glClear(GL_DEPTH_BUFFER_BIT);

            glm::mat4 M, V, P;
            glm::mat4 MVP;
            if (renderState->exists("ProjectionMatrix", "/")) {
                // This is the typical case where the MinVR DisplayGraph contains
                // an OffAxisProjectionNode or similar node, which sets the
                // ProjectionMatrix and ViewMatrix based on head tracking data.
           /* 
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();*/
                VRMatrix4 PP = renderState->getValue("ProjectionMatrix", "/");
                //glLoadMatrixd(PP.m);
                    
              //  glMatrixMode(GL_MODELVIEW);
              //  glLoadIdentity();
              
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

                /*
                P  = glm::mat4(PP[0][0], PP[0][1], PP[0][2], PP[0][3],
                               PP[1][0], PP[1][1], PP[1][2], PP[1][3],
                               PP[2][0], PP[2][1], PP[2][2], PP[2][3],
                               PP[3][0], PP[3][1], PP[3][2], PP[3][3]);
                //P = glm::transpose(P);
                VV = VV * MM;
                V  = glm::mat4(VV[0][0], VV[0][1], VV[0][2], VV[0][3],
                               VV[1][0], VV[1][1], VV[1][2], VV[1][3],
                               VV[2][0], VV[2][1], VV[2][2], VV[2][3],
                               VV[3][0], VV[3][1], VV[3][2], VV[3][3]);
                VRMatrix4 comb = (PP * VV * MM);
                MVP  = glm::mat4(comb[0][0], comb[0][1], comb[0][2], comb[0][3],
                               comb[1][0], comb[1][1], comb[1][2], comb[1][3],
                               comb[2][0], comb[2][1], comb[2][2], comb[2][3],
                               comb[3][0], comb[3][1], comb[3][2], comb[3][3]);
                V = glm::transpose(V);
                P = glm::transpose(P);
                MVP = P * V;*/
                V = glm::make_mat4(VV.m);
                P = glm::make_mat4(PP.m);
                M = glm::make_mat4(MM.m);
            }
            else {
                // If the DisplayGraph does not contain a node that sets the
                // ProjectionMatrix and ViewMatrix, then we must be in a non-headtracked
                // simple desktop mode.  We can just set the projection and modelview
                // matrices the same way as in adaptee.cpp.
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective(1.6*45.f, 1.f, 0.1f, 100.0f);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
              
                double cameraPos[3];
                cameraPos[0] = _radius * cos(_horizAngle) * cos(_vertAngle);
                cameraPos[1] = -_radius * sin(_vertAngle);
                cameraPos[2] = _radius * sin(_horizAngle) * cos(_vertAngle);
    
                double cameraAim[3];
                cameraAim[0] = cos(_horizAngle) * sin(_vertAngle);
                cameraAim[1] = cos(_vertAngle);
                cameraAim[2] = sin(_horizAngle) * sin(_vertAngle);
              
                double targetPos[3] = {0.0f, 0.0f, 0.0f};
               gluLookAt (cameraPos[0], cameraPos[1], cameraPos[2],
                           targetPos[0], targetPos[1], targetPos[2],
                           cameraAim[0], cameraAim[1], cameraAim[2]);
 

                P = glm::perspective(1.6f*45.f, 1.f, 0.1f, 100.0f);
                glm::vec3 pos = glm::vec3(_radius * cos(_horizAngle) * cos(_vertAngle),
                                          -_radius * sin(_vertAngle),
                                          _radius * sin(_horizAngle) * cos(_vertAngle));
                glm::vec3 up = glm::vec3( cos(_horizAngle) * sin(_vertAngle),
                                          cos(_vertAngle),
                                          sin(_horizAngle) * sin(_vertAngle));

                V = glm::lookAt(pos, glm::vec3(0.f), up);
                
    //            V[0] = glm::vec4(0, 0, -1, 0);
      //          V[1] = glm::vec4(0, 1, 0, 0);
        //        V[2] = glm::vec4(1, 0, 0, 0);
          //      V[3] = glm::vec4(0, 0, -.1, 1);


            }

                glm::mat4 scale = glm::scale(glm::mat4(), glm::vec3(20,20,20));
                glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3(1, -1, 0));
                //in desktop mode, +x is away from camera, +z is right, +y is up 
                //M = translate * scale;
                M = M * translate * scale;
                //V = glm::transpose(V);
                MVP = P * V * M ;
                glm::mat4 t = V ;
                frame++;
                _pm->Draw(frame % 40, MVP );
                printf("Printing MVP: \n");
                printMat4(MVP);
		}
	}

	void run() {
		while (!_quit) {
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
};



int main(int argc, char **argv) {

    MyVRApp app(argc, argv);
  	app.run();

	exit(0);
}

