#include <vrg3d/VRG3D.h>
#include "pointmanager.h"

using namespace G3D;

class DinoApp : public VRApp
{
public:
  DinoApp(std::string setup) : VRApp()
  {
    init(setup);
    pm.ReadFile("DataFileTest.txt");
    _shader = Shader::fromFiles("shaders/basic.vert", "shaders/basic.geom", "shaders/basic.frag");
    first = true;
    correct = true;
  }

  void doUserInput(Array<VRG3D::EventRef> &events)
  {


  }

  void doGraphics(RenderDevice *rd)
  {
    while(glGetError() != GL_NO_ERROR)
    {
      std::cout << "Flushing gl errors" << std::endl;
    }
    rd->setShader(_shader);
    float t = _frameCounter % 50;
    pm.Draw(rd, t);
  }
protected:
  PointManager pm;
  ShaderRef       _shader;
  bool first;
  bool correct;
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
