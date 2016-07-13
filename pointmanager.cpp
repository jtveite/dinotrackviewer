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
//    vb = VertexBuffer::create(points.size() * sizeof(Vertex));

    /*
    vr = VertexRange(points.size() * sizeof(Vertex), vb);
    position_range = VertexRange(posArray, vr, 0, sizeof(Vertex));
    color_range = VertexRange(colArray, vr, sizeof(Vector3), sizeof(Vertex));
    printf("Initial Array sizes: pos %d, col %d\n", posArray.size(), colArray.size());
*/

    //position_range = VertexRange(pointArray, vb);
    colorTexture = Image3::fromFile("colormap.jpg");
    computeLocations(50);
    glGenBuffers(1, &buffer);
    glGenVertexArrays(1, &vao);
    pointShader = MyShader("basic.vert", "basic.geom", "basic.frag");
    pointShader.checkErrors();
    //s.loadTexture("colormap.jpg");
    //
    std::vector<int> pl(2);
    pathlines = pl;
    pathlines.clear();
    //AddPathline(Vector3(0.05, 0.2, 0.07), 0);
    const char* all_paths = std::getenv("SHOW_PATHS");
    if (all_paths){
      for(int i = 0; i < points.size(); i++){
        pathlines.push_back(i);
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

void PointManager::separateVertex(Array<Vertex> array){
    posArray.clear();
    colArray.clear();
    for(int i = 0; i < array.size(); i++){
        Vertex v = array[i];
        posArray.append(v.position);
        colArray.append(v.color);
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
  pathlines.push_back(bestID);
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
    rd->endOpenGL();
    rd->popState();
    //printf("Time end of frame: %f\n", ((float)(clock() - startTime)) / CLOCKS_PER_SEC);
    //pathlines.push_back(numFramesSeen);
    for(int i = 0; i < pathlines.size(); i++){
      int pointID = pathlines[i];
      points[pointID].DrawPathline(rd);
    }

}
