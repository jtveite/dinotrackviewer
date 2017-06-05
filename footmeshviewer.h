#include "GLUtil.h"
#include "glm/glm.hpp"
#include "shader.h"

#include <string>
#include <vector>
#include <memory> //For the unique pointer
/**
 * FootMeshViewer is a class that displays a 3D model throughout the animation
 *
 */.
class FootMeshViewer{
public:
  /**
   * Reads a file specifying where to look for the other model files. 
   * Format is 1 line with a prefix that goes in front of each of the files (typically for specifying the directory containing the files.
   * The rest of the lines are a filename per frame
   * @param fileName the name of the file to load
   */
  void ReadFiles(std::string fileName);
  /**
   * Draws the model of the foot at the specified frame.
   * @param time The timestep in the animation to be displayed.
   * @param mvp The Model-View-Projection matrix to be used when displaying the foot.
   */ 
  void Draw(int time, glm::mat4 mvp);



private:
  bool active = true;
  std::vector<glm::vec3> ReadSingleFile(std::string fileName);

  std::vector<GLuint> bufferIds;
  std::unique_ptr<MyShader> shader;
  std::vector<int> numElements;
};
