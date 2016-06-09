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
    return Vertex(p, GetColor(time));
}


Vector4 interp(float val, float from, float to, Vector4 fromVal, Vector4 toVal)
{
  float d = (val - from) / (to - from);
  return fromVal * d + toVal * (1.f - d);
  
}

Vector4 getColorHorizontal(Vector3 pos)
{
    float val = pos.y;
    Vector4 backCol = Vector4(0, .6, 0, 1);
    Vector4 frontCol = Vector4(.6, 0, 1, 1);
    return interp(val, 0.069, 0.21, backCol, frontCol);
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
