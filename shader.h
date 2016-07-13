#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
#include <vrg3d/VRG3D.h>

class MyShader{
public:
  MyShader(){};
  MyShader(std::string vertName, std::string geomName, std::string fragName);

  MyShader & operator=(const MyShader & other)
  {
    vertShader = other.vertShader;
    geomShader = other.geomShader;
    fragShader = other.fragShader;
    program    = other.program   ;
    return *this;
  }


  void setMatrix4(std::string argument, G3D::Matrix4 mat);
  void setFloat(std::string argument, float val);

  void loadTexture(std::string fileName);

  void bindShader();
  void unbindShader();

  void checkErrors();
  void checkError(GLint shader_id);
  void checkProgramError();

private:

  std::string readFile(std::string name);
  
  void setShaders(std::string, std::string, std::string);
  
 // G3D::Texture::Ref texture;
  GLuint vertShader;
  GLuint geomShader;
  GLuint fragShader;

  GLuint program;

  bool hasTexture = false;
};
