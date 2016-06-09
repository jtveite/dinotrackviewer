#include <vrg3d/VRG3D.h>
#include "pointmanager.h"

using namespace G3D;

class DinoApp : public VRApp
{
public:
  DinoApp(std::string setup) : VRApp()
  {
    Log *dinolog = new Log("dino-log.txt");
    init(setup, dinolog);

    pm.ReadFile("/users/jtveite/data/jtveite/slices-130.out");
//    pm.ReadFile("/users/jtveite/dinotrackviewer/test-data");
    _shader = Shader::fromFiles("basic.vert",  "basic.frag");

    std::cout << GL_NO_ERROR << ' ' << GL_INVALID_ENUM << ' ' << GL_INVALID_VALUE << ' ' << GL_INVALID_OPERATION << ' ' << GL_INVALID_FRAMEBUFFER_OPERATION << std::endl;
  }

  virtual ~DinoApp() {}

  void doUserInput(Array<VRG3D::EventRef> &events)
  {
    
  }
 

  void doGraphics(RenderDevice *rd)
  {
    std::cout << "Rendering frame: " << _frameCounter << std::endl;
    rd->pushState();
    //rd->setObjectToWorldMatrix(_virtualToRoomSpace);
    int gl_error;
    while((gl_error = glGetError()) != GL_NO_ERROR)
    {

      std::cout << "Flushing gl errors " << gl_error << std::endl;
    }
    float t = _frameCounter % 50;
    _shader = Shader::fromFiles("basic.vert", "basic.geom",  "basic.frag");
    _shader->args.set("time", t);
   
    Matrix3 scale = Matrix3::fromDiagonal(Vector3(20, 20, 20));
    CoordinateFrame scaleC (scale);
    CoordinateFrame rotate = CoordinateFrame::fromXYZYPRDegrees(
      1, 0, -7,
      0, -90, 180);
    rd->pushState();
    rd->setObjectToWorldMatrix(rotate * scale * rd->objectToWorldMatrix());
    Matrix4 mvp = rd->invertYMatrix() * rd->modelViewProjectionMatrix();
    
    _shader->args.set("mvp", mvp);

    //std::cout << t << std::endl;
    rd->setShader(_shader);
    //std::cout << _shader->messages();
    pm.Draw(rd, t, mvp);
    rd->popState();

    Vector3 hp = getCamera()->getHeadPos();
   // printf("Head position: %f, %f, %f\n", hp.x, hp.y, hp.z);
    Vector3 lv = getCamera()->getLookVec();
  //  printf("Look Vector: %f, %f, %f\n", lv.x, lv.y, lv.z);

    CoordinateFrame owm = rd->modelViewMatrix();
  //  std::cout << owm.toXML() << std::endl;
    rd->popState();
  }

protected:
  PointManager pm;
  ShaderRef _shader;
  MouseToTrackerRef _mouseToTracker;
  CoordinateFrame _virtualToRoomSpace;
  Table<std::string, CoordinateFrame> _trackerFrames;
};

int main(int argc, char **argv )
{
  std::string setup;
  if (argc >= 2)
  {
    setup = std::string(argv[1]);
  }


  DinoApp *app = new DinoApp(setup);
  app->run();
  return 0;

}
