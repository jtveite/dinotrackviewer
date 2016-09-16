#include "vrpoint.h"
#include <stdio.h>

using namespace G3D;

VRPoint::VRPoint(int id)
{
    m_id = id;
}

Vertex VRPoint::Draw(int time, glm::vec3 minV, glm::vec3 maxV)
{
    if (time > positions.size()){
        return Vertex();
        //Fail silently
    }
    glm::vec3 p = positions[time];
   // printf("Drawing point at %f, %f, %f.\r", p.x, p.y, p.z);
    //Sphere s (p, 1e-4);
    //Draw::sphere(s, rd);
 
    //rd->sendVertex(p);
    return Vertex(p, GetColor(time, minV, maxV));
}


glm::vec4 interp(float val, float from, float to, glm::vec4 fromVal, glm::vec4 toVal)
{
  float d = (val - from) / (to - from);
  return fromVal * d + toVal * (1.f - d);
  
}


glm::vec4 getColorHorizontal(glm::vec3 pos)
{
    float val = pos.y;
    glm::vec4 backCol = glm::vec4(0, .6, 0, 1);
    glm::vec4 frontCol = glm::vec4(.6, 0, 1, 1);
    return interp(val, 0.069, 0.21, backCol, frontCol);
}


glm::vec4 VRPoint::GetColorHorizontalPosition()
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

glm::vec2 VRPoint::GetColor(int time, glm::vec3 minV, glm::vec3 maxV)
{
  glm::vec3 pos = positions[0];
  float x = interpi(pos.x, minV.x, maxV.x, 0, 1);
  float y = interpi(pos.y, minV.y, maxV.y, 0, 1);
  float z = interpi(pos.z, minV.z, maxV.z, 0, 1);
  return glm::vec2(z, y);
  // return GetColorHorizontalPosition(); 

}

void VRPoint::AddPoint(glm::vec3 point)
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

float VRPoint::GetDistance(int time, glm::vec3 point)
{
  return (point - positions[time]).squaredMagnitude();
}


glm::vec3 getVertexPosition(glm::vec3 right, glm::vec3 up, glm::vec3 base, double radius, double theta){
  return base + radius * cos(theta) * up + radius * sin(theta) * right;
}


glm::vec2 VRPoint::getColor(int position){
  float pos = interpi(position, 0, positions.size(), 0, 1);
  return glm::vec2(pos, 0.5);
}

std::vector<Vertex> VRPoint::getPathlineVerts()
{
  double radius = 0.0001;
  std::vector<Vertex> v;
  std::vector<Vertex> points;
  std::vector<int> indices;
  int steps_around = 20;
  for(int i = 0; i < positions.size() -1; i++){
    glm::vec3 forward = positions[i+1] - positions[i];
    glm::vec3 wup = glm::vec3(0, 0, 1);
    glm::vec3 right = wup.cross(forward).direction();
    glm::vec3 up = forward.cross(right).direction();
    glm::vec2 col = getColor(i);
    for(int j = 0; j < steps_around; j++){
      points.push_back(Vertex(getVertexPosition(right, up, positions[i], radius, (j * 6.28 / steps_around)), col));
    }
    for (int j = 0; j < steps_around - 1; j++){
      int base = steps_around * i +j;
      indices.push_back(base);
      indices.push_back(base + steps_around);

      indices.push_back(base + steps_around + 1);
      indices.push_back(base + 1);
    }
    int j = steps_around - 1;
    int base = steps_around * i + j;
    indices.push_back(base);
    indices.push_back(base + steps_around);
    indices.push_back(base + 1);
    indices.push_back(base - steps_around + 1);

  }
  int i = positions.size()-1;
  glm::vec3 forward = positions[i] - positions[i-1];
  glm::vec3 wup = glm::vec3(0,0,1);
  glm::vec3 right = wup.cross(forward).direction();
  glm::vec3 up = forward.cross(right).direction();
  glm::vec2 col = getColor(i);
  for(int j = 0; j < steps_around; j++){
    points.push_back(Vertex(getVertexPosition(right, up, positions[i], radius, j * 1.0 / steps_around), col));
  }

  for(int i = 0; i < indices.size(); i++){
    v.push_back(points[indices[i]]);
  }
  return v;
}


int VRPoint::steps()
{
  return positions.size();
}


float VRPoint::totalPathLength()
{
  float totalDistance = 0;
  for(int i = 0; i < positions.size() - 1; i++){
    float d = (positions[i] - positions[i+1]).magnitude();
    totalDistance += d;
  }
  return totalDistance;
}

bool VRPoint::withinDistance(VRPoint& other, double distance)
{
  int l1 = positions.size();
  int l2  = other.positions.size();
  int length = std::min(l1, l2);
  double d = distance * distance;
  for(int i = 0; i < positions.size(); i++){
    for(int j = 0; j < other.positions.size(); j++){
      float dist = (positions[i] - other.positions[j]).magnitude();
      if ( dist < distance){
        return true;
      }
    }
  }
  return false;
}
