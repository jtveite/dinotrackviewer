#include "shader.h"
#include <string>
#include <fstream>
#include <sstream>



MyShader::MyShader(std::string vertName, std::string geoName, std::string fragName){
    usingGeom = true;
    setShaders( vertName, geoName, fragName);
}

MyShader::MyShader(std::string vertName, std::string fragName){
    usingGeom = false;
    setShaders( vertName, "", fragName);
}

std::string MyShader::readFile(std::string name){
    std::ifstream t(name.c_str());
    std::stringstream buf;
    buf << t.rdbuf();
    return buf.str();
}


void MyShader::setShaders(std::string vertName, std::string geoName, std::string fragName){
    std::string vertString = readFile(vertName);
    std::string fragString = readFile(fragName);
    vertShader = glCreateShader(GL_VERTEX_SHADER);
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *cs = vertString.c_str();
    glShaderSource(vertShader, 1, &cs, NULL);
    cs = fragString.c_str();
    glShaderSource(fragShader, 1, &cs, NULL);

    if (usingGeom){
      std::string geomString = readFile(geoName);
      geomShader = glCreateShader(GL_GEOMETRY_SHADER);
      cs = geomString.c_str();
      glShaderSource(geomShader, 1, &cs, NULL);
    } 
    
    glCompileShader(vertShader);
    if (usingGeom){
      glCompileShader(geomShader);
    }
    glCompileShader(fragShader);

    program = glCreateProgram();

    glAttachShader(program, vertShader);
    if (usingGeom){
      glAttachShader(program, geomShader);
    }
    glAttachShader(program, fragShader);

    glLinkProgram(program);

}

void MyShader::setMatrix4(std::string argument, G3D::Matrix4 mat)
{
  float* vals = mat;
  GLint loc = glGetUniformLocation(program, argument.c_str());
  glUniformMatrix4fv(loc, 1, GL_TRUE, vals);
}

void MyShader::setFloat(std::string argument, float val)
{
  GLint loc = glGetUniformLocation(program, argument.c_str());
  glUniform1f(loc, val);
}

void MyShader::bindShader(){
  glUseProgram(program);
  if(hasTexture){
    GLint screenLoc = glGetUniformLocation(program, textureName.c_str());
    glUniform1i(screenLoc, 0);
    GLint u = texture->openGLTextureTarget();
    glEnable(u);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(u, texture->openGLID());
  }

}

void MyShader::unbindShader(){
    glUseProgram(0);
} 


void MyShader::checkErrors(){
  checkError(vertShader);
  checkError(fragShader);
  if (usingGeom){
    checkError(geomShader);
  }
  checkProgramError();
}

void MyShader::checkError(GLint sid){
  GLint isCompiled = 0;
  glGetShaderiv(sid, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE){
    GLint maxLength = 0;
    glGetShaderiv(sid, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(sid, maxLength, &maxLength, &errorLog[0]);

    std::cout << "Error compiling shader: " << std::endl;
    printf("%s\n", &errorLog[0]);
  }
}

void MyShader::checkProgramError(){
  GLint isLinked = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
  if(isLinked == GL_FALSE){
    GLint maxLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<GLchar> infoLog(maxLength);
    glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

    printf("Error linking shader program:\n%s\n", &infoLog[0]);

  }

}

void MyShader::loadTexture(std::string argument, std::string fileName){
 // texture = G3D::Texture::fromFile(fileName);
 // printf("Texture target: %d, texture id: %d\n", texture->openGLTextureTarget(), texture->openGLID());
  texture = G3D::Texture::fromFile(fileName);
  textureName = argument;
  hasTexture = true;
}
