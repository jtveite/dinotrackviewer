#pragma once

#if defined(__APPLE__)
#include <GL/glew.h>
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <string>
#include "glm/glm.hpp"
#include <unordered_map>

struct Texture{
public:
  Texture(std::string texName, GLuint texTarget, GLuint texID) {
    name = texName;
    target = texTarget;
    ID = texID;
  }
  Texture() {};
  std::string name;
  GLuint target;
  GLuint ID;
};

class MyShader{
public:
  MyShader(){};
  MyShader(std::string vertName, std::string geomName, std::string fragName);
  MyShader(std::string vertName, std::string fragName);

  MyShader & operator=(const MyShader & other)
  {
    vertShader = other.vertShader;
    geomShader = other.geomShader;
    fragShader = other.fragShader;
    program    = other.program   ;
    return *this;
  }


  void setMatrix4(std::string argument, glm::mat4 mat);
  void setFloat(std::string argument, float val);

  void loadTexture(std::string argument, std::string fileName);

  void bindShader();
  void unbindShader();

  void checkErrors();
  void checkError(GLint shader_id);
  void checkProgramError();
  bool usingGeom;

private:


  std::string readFile(std::string name);
  
  void setShaders(std::string, std::string, std::string);
  

  GLuint textureTarget;
  GLuint textureID;
  std::string textureName;
  std::unordered_map<std::string, Texture> textures;
  GLuint vertShader;
  GLuint geomShader;
  GLuint fragShader;

  GLuint program;

  bool hasTexture = false;
};

