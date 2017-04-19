#include "GLUtil.h"
#include "glm/glm.hpp"
#include "pointmanager.h"
#include <vector>
#include <string>

class Slide{
public:
  Slide()  {};
  Slide(std::string texture, glm::vec3 bl, glm::vec3 right, glm::vec3 up);
  void Initialize(std::string texture, glm::vec3 bl, glm::vec3 right, glm::vec3 up);

  void Draw(glm::mat4 mvp);

private:
  std::vector<glm::vec3> verts;
  std::vector<glm::vec2> uvs;
  bool initialized = false;
  MyShader* shader;

  GLuint vertBuffer;
  GLuint uvBuffer;

  int numUVs;
  int numVerts;
};
