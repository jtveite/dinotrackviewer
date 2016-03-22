#include "vrpoint.h"
#include <stdio.h>

using namespace G3D;

VRPoint::VRPoint(int id)
{
    m_id = id;
}

void VRPoint::Draw(RenderDevice *rd, int time)
{
    if (time > positions.size()){
        return;
        //Fail silently
    }
    Vector3 p = positions[time];
   // printf("Drawing point at %f, %f, %f.\r", p.x, p.y, p.z);
    //Sphere s (p, 1e-4);
    //Draw::sphere(s, rd);
    rd->sendVertex(p);


}

void VRPoint::AddPoint(Vector3 point)
{
    positions.push_back(point);
}
