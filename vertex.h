#ifndef VERTEX_H
#define VERTEX_H
#include <GL/gl.h>
#include <vrg3d/VRG3D.h>

using namespace G3D;
struct Vertex{
public:
  Vector3 position;
  Vector2 color;
  Vertex(Vector3 pos, Vector2 col){
    position = pos;
    color = col;
  }
  Vertex(Vector2 pos){
    position = Vector3(pos);
    color = Vector2(0.0, 0.0);
  }
  Vertex(){
    position = Vector3(0.0, 0.0, 0.0);
    color = Vector2(0.0, 0.0);
  }
};

#endif // POINTMANAGER_H
