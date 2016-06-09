#include <vrg3d/VRG3D.h>

using namespace G3D;

class DinoApp : public VRApp
{
public:
  DinoApp(const std::string &setup) : VRApp()
  {
    Log *dinolog = new Log("dino-log.txt");
    init(setup, dinolog);
    _shader = Shader::fromFiles("",  "basic.frag");

  }

  virtual ~DinoApp() {}

  void doUserInput(Array<VRG3D::EventRef> &events)
  {
    for (int i = 0; i < events.size(); i++)
    {
      if (events[i]->getName() == "kbd_ESC_down")
      {
        exit(0);
      }
      else{
        //std::cout << events[i]->getName() << std::endl;
      }

    }

  }

  void doGraphics(RenderDevice *rd)
  {
    rd->pushState();
    int gl_error;
    while(gl_error = glGetError() != GL_NO_ERROR)
    {

    //  std::cout << "Flushing gl errors" << gl_error << std::endl;
    }
    rd->setShader(_shader);
    Sphere s (Vector3(0,0,2), 0.5);
    Draw::sphere(s, rd);
    rd->popState();
  }
protected:
  ShaderRef _shader;
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
