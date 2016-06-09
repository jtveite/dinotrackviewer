#include "vrpoint.h"
#include <stdio.h>

using namespace G3D;

VRPoint::VRPoint(int id)
{
    m_id = id;
}

Vertex VRPoint::Draw(int time)
{
    if (time > positions.size()){
        return Vertex();
        //Fail silently
    }
    Vector3 p = positions[time];
   // printf("Drawing point at %f, %f, %f.\r", p.x, p.y, p.z);
    //Sphere s (p, 1e-4);
    //Draw::sphere(s, rd);
 
    //rd->sendVertex(p);
    return Vertex(p, Vector4(1.0, 0.0, 0.0, 1.0));//, GetColor(time));
}


Vector4 getColorHorizontal(Vector3 pos)
{
    float val = pos.y;
    val *= 5;
    return Vector4(val, val, val, val);
}


Vector4 VRPoint::GetColorHorizontalPosition()
{
    return getColorHorizontal(positions[0]);

}

Vector4 VRPoint::GetColor(int time)
{
   return GetColorHorizontalPosition(); 

}

void VRPoint::AddPoint(Vector3 point)
{
    positions.push_back(point);
}
