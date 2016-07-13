#include "pointmanager.h"
#include "vrpoint.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>

PointManager::PointManager()
{

}

void PointManager::ReadFile(std::string fileName)
{
    std::fstream file;
    std::string line;
    file.open(fileName, std::ios::in);
    int linecount = 0;
    while(std::getline(file, line))
    {
        std::istringstream iss(line);
        int id;
        float x,y,z;
        iss >> id;
        VRPoint p (id);
        while (iss >> x >> y >> z){
            p.AddPoint(Vector3(x,y,z));
        }
        points.push_back(p);
    }
    colorTexture = Image3::fromFile("colormap.jpg");
    pathTexture = Image3::fromFile("pathmap.jpg");
    computeLocations(50);
    glGenBuffers(1, &buffer);
    glGenBuffers(1, &pathBuffer);
    glGenVertexArrays(1, &vao);
    pointShader = MyShader("shaders/basic.vert", "shaders/basic.geom", "shaders/basic.frag");
    pointShader.checkErrors();
    lineShader = MyShader("shaders/path.vert", "shaders/path.frag");
    //s.loadTexture("colormap.jpg");
    //
    std::vector<int> pl(2);
    pathlines = pl;
    pathlines.clear();
    //AddPathline(Vector3(0.05, 0.2, 0.07), 0);
    const char* all_paths = std::getenv("SHOW_PATHS");
    if (all_paths){
      for(int i = 0; i < points.size(); i++){
 
        int offset = pathVertices.size();
        pathOffsets.push_back(offset);
        std::vector<Vertex> newVerts = points[i].getPathlineVerts(pathTexture);
        pathCounts.push_back(newVerts.size());
        pathVertices.insert(pathVertices.end(), newVerts.begin(), newVerts.end());

      }
    }
}

void PointManager::computeLocations(int timesteps){
  pointLocations.clear();
  for(int i = 0; i < timesteps; i++){
    std::vector<Vertex> pointArray;
    for (int j = 0; j < points.size(); j++){
      VRPoint point = points[j];
      pointArray.push_back(point.Draw(i, colorTexture));
    }
    pointLocations.push_back(pointArray);
  }
}

void PointManager::AddPathline(Vector3 pos, int time){
  float minDist = 10000.f;
  int bestID = 0;
  float d;
  //printf("Finding nearest pathline to %f, %f, %f\n", pos.x, pos.y, pos.z);
  for(int i = 0; i < points.size(); i++){
    d = points[i].GetDistance(time, pos);
    if (d < minDist){
      minDist = d;
      bestID = i;
    }
  }

  int offset = pathVertices.size();
  pathOffsets.push_back(offset);
  std::vector<Vertex> newVerts = points[bestID].getPathlineVerts(pathTexture);
  pathCounts.push_back(newVerts.size());
  pathVertices.insert(pathVertices.end(), newVerts.begin(), newVerts.end());

  //printf("Best pathline was for point %d near to %f, %f, %f", bestID, pos.x, pos.y, pos.z);
  //std::cout << std::endl;
}


void PointManager::Draw(RenderDevice *rd, int time, Matrix4 mvp){
    numFramesSeen++;
    clock_t startTime = clock();
    rd->pushState();
      std::vector<Vertex> *pointArray = &pointLocations[time];
    //printf("Time after setting points: %f\n", ((float)(clock() - startTime)) / CLOCKS_PER_SEC);
    rd->beginOpenGL();
    pointShader.bindShader();
    pointShader.setMatrix4("mvp", mvp);
    pointShader.setFloat("rad", pointSize);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    int bufferSize = pointArray->size() * sizeof(Vertex);
    Vertex* bufferData = pointArray->data();
    glBufferData(GL_ARRAY_BUFFER, bufferSize, bufferData, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,  sizeof(Vertex), NULL);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) sizeof(Vector4));
    //printf("Time before draw call: %f\n", ((float)(clock() - startTime)) / CLOCKS_PER_SEC);
    glDrawArrays(GL_POINTS, 0, pointArray->size());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    pointShader.unbindShader();

    lineShader.bindShader();
    lineShader.setMatrix4("mvp", mvp);
    
    
    bufferSize = pathVertices.size() * sizeof(Vertex);
    bufferData = pathVertices.data();
    glBindBuffer(GL_ARRAY_BUFFER, pathBuffer);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, bufferData, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,  sizeof(Vertex), NULL);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) sizeof(Vector4));
    glMultiDrawArrays(GL_LINE_STRIP, pathOffsets.data(), pathCounts.data(), pathOffsets.size());
    lineShader.unbindShader();
    rd->endOpenGL();
    rd->popState();
    //printf("Time end of frame: %f\n", ((float)(clock() - startTime)) / CLOCKS_PER_SEC);
    //pathlines.push_back(numFramesSeen);
    for(int i = 0; i < pathlines.size(); i++){
      int pointID = pathlines[i];
      //points[pointID].DrawPathline(rd);
    }

}
