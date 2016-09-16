#ifndef VERTEX_H
#define VERTEX_H
#include <GL/gl.h>
#include <vrg3d/VRG3D.h>
#include <glm/glm.hpp>


using namespace G3D;
struct Vertex{
public:
  glm::vec3 position;
  glm::vec2 color;
  Vertex(glm::vec3 pos, glm::vec2 col){
    position = pos;
    color = col;
  }
  Vertex(glm::vec3 pos){
    position = pos;
    color = glm::vec2(0.0, 0.0);
  }
  Vertex(){
    position = glm::vec3(0.0, 0.0, 0.0);
    color = glm::vec2(0.0, 0.0);
  }
};

#endif // POINTMANAGER_H
