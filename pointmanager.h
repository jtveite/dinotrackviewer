#ifndef POINTMANAGER_H
#define POINTMANAGER_H
#include <vector>
#include <string>
#include "vrpoint.h"


class PointManager
{
public:
    PointManager();
    void ReadFile(std::string fileName);
    void Draw(RenderDevice *rd);

private:
    std::vector<VRPoint> points;
};

#endif // POINTMANAGER_H
