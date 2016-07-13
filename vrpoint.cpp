#include "vrpoint.h"
#include <stdio.h>

using namespace G3D;

VRPoint::VRPoint(int id)
{
    m_id = id;
}

Vertex VRPoint::Draw(int time, Image3::Ref image)
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
    return Vertex(p, GetColor(time, image));
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

float interpi(float val, float from, float to, int fromVal, int toVal)
{
  float d = (1.0 * val - from) / (1.0f * to - from);
  return  fromVal *  d + toVal * (1.f - d);
}


float bound(float x){
  if (x < 0){
    return 1;
  }
  else if (x > 511){
    return 511;
  }
  else
    return x;
}

Vector4 VRPoint::GetColor(int time, Image3::Ref image)
{
  Vector3 pos = positions[0];
  float x = interpi(pos.x, 0, 0.22, 0, 512 - 1);
  float y = interpi(pos.y, 0, 0.12, 0, 512 - 1);
  float z = interpi(pos.z, -0.048, 0.076, 0, 512-1);
  x = bound(x);
  y = bound(y);
  z = bound(z);
  //printf("Pixel grabbing from %f,%f with coords %f,%f\n", x, y, pos.y, pos.x);
  Color3 color = image->nearest(z,y);
  return Vector4(Vector3(color), 1.0f);
  // return GetColorHorizontalPosition(); 

}

void VRPoint::AddPoint(Vector3 point)
{
    positions.push_back(point);
}

void VRPoint::DrawPathline(RenderDevice *rd)
{
  if (glGetError() != GL_NO_ERROR){
    printf("bad at start\n");
  }
  rd->beginPrimitive(PrimitiveType::LINE_STRIP);
  //printf("Drawing pathline with the first vertex at %f, %f, %f\n", positions[0].x, positions[0].y, positions[0].z);
  for(int i = 0; i < positions.size(); i++){
    rd->sendVertex(positions[i]);
  }
  rd->endPrimitive();
  if (glGetError() != GL_NO_ERROR){
    printf("bad at end\n");
  }
}

float VRPoint::GetDistance(int time, Vector3 point)
{
  return (point - positions[time]).squaredMagnitude();
}

std::vector<Vertex> VRPoint::getPathlineVerts(Image3::Ref image)
{
  std::vector<Vertex> v;
  for(int i = 0; i < positions.size(); i++){
    float pos = interpi(i, 0, positions.size(), 0, 512-1);
    Color3 col = image->nearest(pos, 0);
    Color4 vv (col, 1.0);
    v.push_back(Vertex(positions[i], vv));
  }
  return v;
}
