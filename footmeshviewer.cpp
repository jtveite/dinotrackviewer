#include "footmeshviewer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>


void FootMeshViewer::ReadFiles(std::string fileName){
  printf("READING FILES FOR FMV\n");
  std::fstream file;
  file.open(fileName, std::ios::in);
  std::string prefix;
  std::string line;
  std::vector<std::string> fileNames;

  std::getline(file, prefix);
  printf("prefix is %s\n", prefix.c_str());



  while (std::getline(file, line)){
    fileNames.push_back(prefix + line); 
  }
  
  int timeSteps = fileNames.size();
  if (timeSteps == 0){
    active = false;
    return;
  }
  GLuint vbos[timeSteps];
  glGenBuffers(timeSteps, vbos);
  bufferIds.assign(vbos, vbos+timeSteps);
  for (int i = 0; i < fileNames.size(); ++i){
    //Get a list of vertices to send
    const std::vector<glm::vec3>& tris = ReadSingleFile(fileNames[i]);
    //Bind the correct buffer
    glBindBuffer(GL_ARRAY_BUFFER, bufferIds[i]);
    int numElems = tris.size();
    int bufferSize = numElems * sizeof(glm::vec3);
    numElements.push_back(numElems);
    //Send the data to the buffer
    glBufferData(GL_ARRAY_BUFFER, bufferSize, tris.data(), GL_STATIC_DRAW);
    glCheckError();
    //Unbind or else opengl gets mad
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

 
  //Initialize the shader
  shader = std::make_unique<MyShader>("shaders/foot.vert", "shaders/foot.geom",  "shaders/foot.frag");
  //shader = std::make_unique<MyShader>("shaders/foot.vert",  "shaders/foot.frag");
  shader->checkErrors();
}

#define SKIP(N) for (int iii = 0; iii < N; ++iii) { std::getline(file, line); }

std::vector<glm::vec3> FootMeshViewer::ReadSingleFile(std::string fileName){
  std::fstream file;
  file.open(fileName, std::ios::in);
  
  std::string line;
  std::vector<glm::vec3> locations;
  std::vector<int> indices;

  SKIP(4);
  std::getline(file, line);//Should be "POINTS nnnnnn float"
  std::istringstream counter(line);
  std::string dummy;
  int numPoints;
  counter >> dummy >> numPoints;
  for (int i = 0; i < numPoints; ++i){
    std::getline(file, line);
    float x,y,z;
    std::istringstream iss(line);
    iss >> x >> y >> z;
    locations.push_back(glm::vec3(x,y,z));
  }
  
  SKIP(1);
  std::istringstream tricounter(line);
  int numTris;
  tricounter >> dummy >>  numTris;
  

  for (int i = 0; i < numTris; ++i){
    std::getline(file, line);
    int f,a,b,c;
    std::istringstream iss(line);
    iss >> f >> a >> b >> c;
    indices.push_back(a);
    indices.push_back(b);
    indices.push_back(c);
  }

  std::vector<glm::vec3> finalVertices;
  for (int i = 0; i < indices.size(); ++i){
    finalVertices.push_back(locations[indices[i]]);
  }

  return finalVertices;
}


void FootMeshViewer::Draw(int time, glm::mat4 mvp){
  if (!active){
    return;
  }
  if (time < 0 || time >= bufferIds.size()){
    return;
  }

  glCheckError();
  shader->bindShader();
  glCheckError();
  shader->setMatrix4("mvp", mvp);
  glCheckError();

  glBindBuffer(GL_ARRAY_BUFFER, bufferIds[time]);
  glCheckError();

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glCheckError();

  glDrawArrays(GL_TRIANGLES, 0, numElements[time]);
  glCheckError();

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  shader->unbindShader();
  glCheckError();
}
