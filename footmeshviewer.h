#include "GLUtil.h"
#include "glm/glm.hpp"
#include "shader.h"

#include <string>
#include <vector>
#include <memory> //For the unique pointer

class FootMeshViewer{
public:

  void ReadFiles(std::string fileName);
  void Draw(int time, glm::mat4 mvp);



private:
  bool active = true;
  std::vector<glm::vec3> ReadSingleFile(std::string fileName);

  std::vector<GLuint> bufferIds;
  std::unique_ptr<MyShader> shader;
  std::vector<int> numElements;
};
