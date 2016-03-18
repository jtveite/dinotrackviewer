#ifndef POINTMANAGER_H
#define POINTMANAGER_H
#include <vector>
#include <string>
#include "vrpoint.h"
#include <vrg3d/VRG3D.h>

using namespace G3D;

class PointManager
{
public:
    PointManager();
    void ReadFile(std::string fileName);
    void Draw(RenderDevice *rd, int time);

private:
    std::vector<VRPoint> points;
};

#endif // POINTMANAGER_H
