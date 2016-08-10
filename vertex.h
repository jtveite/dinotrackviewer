#ifndef VERTEX_H
#define VERTEX_H
#include <GL/gl.h>
#include <vrg3d/VRG3D.h>

using namespace G3D;
struct Vertex{
public:
  Vector3 position;
  Vector3 color;
  Vertex(Vector3 pos, Vector4 col){
    position = pos;
    color = col.xyz();
  }
  Vertex(Vector3 pos){
    position = Vector3(pos);
    color = Vector3(1.0, 1.0, 1.0);
  }
  Vertex(){
    position = Vector3(0.0, 0.0, 0.0);
    color = Vector3(0.0, 0.0, 0.0);
  }
};

#endif // POINTMANAGER_H
