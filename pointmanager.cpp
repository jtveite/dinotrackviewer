#include "pointmanager.h"
#include "vrpoint.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

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
    vb = VertexBuffer::create(points.size() * sizeof(Vertex));

    /*
    vr = VertexRange(points.size() * sizeof(Vertex), vb);
    position_range = VertexRange(posArray, vr, 0, sizeof(Vertex));
    color_range = VertexRange(colArray, vr, sizeof(Vector3), sizeof(Vertex));
    printf("Initial Array sizes: pos %d, col %d\n", posArray.size(), colArray.size());
*/

    //position_range = VertexRange(pointArray, vb);
    computeLocations(50);
    std::cout << std::endl;
    glGenBuffers(1, &buffer);
    glGenVertexArrays(1, &vao);
    s = MyShader("basic.vert", "basic.geom", "basic.frag");
    s.checkErrors();
}

void PointManager::computeLocations(int timesteps){
  pointLocations.clear();
  for(int i = 0; i < timesteps; i++){
    std::vector<Vertex> pointArray;
    for (int j = 0; j < points.size(); j++){
      VRPoint point = points[j];
      pointArray.push_back(point.Draw(i));
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

void PointManager::Draw(RenderDevice *rd, int time, Matrix4 mvp){
    clock_t startTime = clock();
    rd->pushState();
   // std::vector<Vertex> pointArray;
   // pointArray.reserve(points.size());
   /*
    for(int i = 0; i < points.size(); i++){
        VRPoint point = points[i];
        pointArray.push_back(point.Draw(time));
        //pointArray.append(Vector3(0, 0, i));
    }*/
    std::vector<Vertex> pointArray = pointLocations[time];
    //printf("Time after setting points: %f\n", ((float)(clock() - startTime)) / CLOCKS_PER_SEC);
    //separateVertex(pointArray);
    rd->beginOpenGL();
    s.bindShader();
    s.setMatrix4("mvp", mvp);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, pointArray.size() * sizeof(Vertex), pointArray.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,  sizeof(Vertex), NULL);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) sizeof(Vector4));
    //printf("Time before draw call: %f\n", ((float)(clock() - startTime)) / CLOCKS_PER_SEC);
    glDrawArrays(GL_POINTS, 0, pointArray.size());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    s.unbindShader();
    rd->endOpenGL();
    //printf("Time end of frame: %f\n", ((float)(clock() - startTime)) / CLOCKS_PER_SEC);

    /*
    printf("Pos Array size: %d, col size: %d\n", posArray.size(), colArray.size());
    printf("Vertex buffer has %d bytes free and %d allocated.\n", vb->freeSize(), vb->allocatedSize());
    std::cout << std::endl;
    vb->reset();
    printf("Vertex buffer has %d bytes free and %d allocated.\n", vb->freeSize(), vb->allocatedSize());
    std::cout << std::endl;
    vr = VertexRange(points.size() * sizeof(Vertex), vb);
    position_range = VertexRange(posArray, vr, 0, sizeof(Vertex));
    color_range = VertexRange(colArray, vr, sizeof(Vector3), sizeof(Vertex));
    printf("Vertex buffer has %d bytes free and %d allocated.\n", vb->freeSize(), vb->allocatedSize());
    rd->setVertexAttribArray(0, position_range, false);
    rd->setVertexAttribArray(1, color_range, false);
    rd->sendSequentialIndices(PrimitiveType::POINTS, pointArray.size());*/
    rd->popState();
}
