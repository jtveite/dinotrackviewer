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
  }

  void doUserInput(Array<VRG3D::EventRef> &events)
  {


  }

  void doGraphics(RenderDevice *rd)
  {
    rd->pushState();
    pm.Draw(rd, 30);
    rd->popState();

  }
protected:
  PointManager pm;
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
