#ifndef VERTEX_H
#define VERTEX_H
#include <GL/gl.h>
#include <vrg3d/VRG3D.h>

using namespace G3D;
struct Vertex{
public:
  Vector4 position;
  Vector4 color;
  Vertex(Vector3 pos, Vector4 col){
    position = Vector4(pos, 1.0);
    color = col;
  }
  Vertex(Vector3 pos){
    position = Vector4(pos, 1.0);
    color = Vector4(1.0, 1.0, 1.0, 1.0);
  }
  Vertex(){
    position = Vector4(0.0, 0.0, 0.0, 0.0);
    color = Vector4(0.0, 0.0, 0.0, 0.0);
  }
};

#endif // POINTMANAGER_H
