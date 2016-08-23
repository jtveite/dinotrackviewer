#include "pointmanager.h"
#include "vrpoint.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>

#include "gperftools/profiler.h"
#include "fast_atof.c"

PointManager::PointManager()
{

}

void PointManager::ReadFile(std::string fileName, bool debug)
{

    clock_t startTime = clock();
    std::ios_base::sync_with_stdio(false);
    std::fstream file;
    std::string line;
    file.open(fileName, std::ios::in);
    int linecount = 0;
    //VERY VERY TEMPORARY
     
    while(std::getline(file, line))
    {
        linecount++;
        if(debug){
          if (linecount % 10000 == 0){
            printf("Currently reading line %d.\r\n", linecount); 
            //ProfilerFlush();
          }
        }
        std::istringstream iss(line);
        int id;
        float x,y,z;
        if (iss.peek() == 'B'){
          std::string c;
          iss >> c;
          //Reading in a box coords
          iss >> x >> y >> z;
          Vector3 lower(x,y,z);
          iss >> x >> y >> z;
          Vector3 upper(x,y,z);
          AABox box (lower.min(upper), upper.max(lower));
          //printf("Read box %f,%f,%f:%f,%f,%f\n", lower.x, lower.y, lower.z, upper.x, upper.y, upper.z );
          boxes.push_back(box);
          continue;
        }
        iss >> id;
        VRPoint p (id);
        /*
        while (iss >> x >> y >> z){
            p.AddPoint(Vector3(x,y,z));
        }
*/
      
        iss.get();//Clear the leading space left over from grabbing the first one
        char token[20];
        float arr[3];
        int idx = 0;
        while(!(iss.eof())){
          iss.getline(token, 20, ' ');
          arr[idx] = atof(token);
          idx++;
          if(idx == 3){
            idx = 0;
            p.AddPoint(Vector3(arr));
          }
        }
        points.push_back(p);
        
    }
    printf("Time after reading points: %f\n", ((float)(clock() - startTime)) / CLOCKS_PER_SEC);


    timeSteps = 0;
    minV = points[0].positions[0];
    maxV = minV;
    for(int i = 0; i < points.size(); i++){
      //timeSteps = max(timeSteps, points[i].steps());  
      if (points[i].steps() > timeSteps){
        timeSteps = points[i].steps();
      }
      minV = minV.min(points[i].positions[0]);
      maxV = maxV.max(points[i].positions[0]);
    }
    


}

void PointManager::SetupDraw(bool allPaths){
    clock_t startTime = clock();
    colorTexture = Image3::fromFile("colormap.jpg");
    pathTexture = Image3::fromFile("pathmap.jpg");
  
    filter = new Filter();
    retestVisible();

    glGenBuffers(1, &buffer);
    glGenBuffers(1, &pathBuffer);
    glGenVertexArrays(1, &vao);
    pointShader = MyShader("shaders/basic.vert", "shaders/basic.geom", "shaders/basic.frag");
    pointShader.checkErrors();
    pointShader.loadTexture("colorMap", "colormap.jpg");
    lineShader = MyShader("shaders/path.vert", "shaders/path.frag");
    lineShader.checkErrors();
    lineShader.loadTexture("pathMap", "pathmap.jpg");

    //s.loadTexture("colormap.jpg");
    //
    //std::vector<int> pl(2);
    //pathlines = pl;
    //pathlines.clear();
    //AddPathline(Vector3(0.05, 0.2, 0.07), 0);
    if (allPaths){
      for(int i = 0; i < points.size(); i++){
 
        int offset = pathVertices.size();
        pathOffsets.push_back(offset);
        std::vector<Vertex> newVerts = points[i].getPathlineVerts();
        pathCounts.push_back(newVerts.size());
        pathVertices.insert(pathVertices.end(), newVerts.begin(), newVerts.end());

      }
    }
   printf("Time after arranging points: %f\n", ((float)(clock() - startTime)) / CLOCKS_PER_SEC);
}

void PointManager::ReadPathlines(std::string fileName){
  std::fstream file;
  file.open(fileName, std::ios::in);
  int id;
  while (file >> id)
  {
    for(int i = 0; i < points.size(); i++){
      if (points[i].m_id == id){
        AddPathline(points[i]);
        break;
      }
    }
  }

} 

int PointManager::getLength(){
  return timeSteps;
}

void PointManager::computeLocations(){
  pointLocations.clear();
  for(int i = 0; i < timeSteps; i++){
    std::vector<Vertex> pointArray;
    for (int j = 0; j < visiblePoints.size(); j++){
      //VRPoint point = points[visiblePoints[j]];
      pointArray.push_back(points[visiblePoints[j]].Draw(i, minV, maxV));
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
  AddPathline(points[bestID]);
  //printf("Best pathline was for point %d near to %f, %f, %f", bestID, pos.x, pos.y, pos.z);
  //std::cout << std::endl;
}

void PointManager::AddPathline(VRPoint& point){
  int offset = pathVertices.size();
  pathOffsets.push_back(offset);
  std::vector<Vertex> newVerts = point.getPathlineVerts();
  pathCounts.push_back(newVerts.size());
  pathVertices.insert(pathVertices.end(), newVerts.begin(), newVerts.end());
  updatePaths = true;
}

void PointManager::SetFilter(Filter* f){
  filter = f;
  retestVisible();
}

void PointManager::retestVisible(){
  visiblePoints.clear();
  for(int i = 0; i < points.size(); i++){
    if (filter->checkPoint(points[i])){
      visiblePoints.push_back(i);
    }
  }
  computeLocations();
}


void PointManager::DrawBoxes(RenderDevice* rd){
  for(int i = 0; i < boxes.size(); i++){
    AABox box = boxes[i];
    float gray = 0.4;
    Draw::box(box, rd, Color4::clear(), Color4(gray, gray, gray, 1.));
  }
}

void PointManager::DrawPoints(int time, Matrix4 mvp){
    //Bind shader and set args
    pointShader.bindShader();
    pointShader.setMatrix4("mvp", mvp);
    pointShader.setFloat("rad", pointSize);

    //Bind and resend buffer data
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    std::vector<Vertex> *pointArray = &pointLocations[time];
    int bufferSize = pointArray->size() * sizeof(Vertex);
    Vertex* bufferData = pointArray->data();
    glBufferData(GL_ARRAY_BUFFER, bufferSize, bufferData, GL_DYNAMIC_DRAW);

    //set vertex attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex), NULL);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) sizeof(Vector3));

    //Draw the points
    glDrawArrays(GL_POINTS, 0, pointArray->size());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Unbind shader
    pointShader.unbindShader();
}

 
void PointManager::DrawPaths(int time, Matrix4 mvp){
    //Bind shader and set args
    lineShader.bindShader();
    lineShader.setMatrix4("mvp", mvp);

    //Bind buffer, resend data if needed
    glBindBuffer(GL_ARRAY_BUFFER, pathBuffer);
    //updatePaths ensures that we only write new paths to the gpu when needed
    if (updatePaths){
      updatePaths = false;
      int bufferSize = pathVertices.size() * sizeof(Vertex);
      Vertex* bufferData = pathVertices.data();
      glBufferData(GL_ARRAY_BUFFER, bufferSize, bufferData, GL_DYNAMIC_DRAW);
    }

    //set vertex attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex), NULL);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) sizeof(Vector3));

    //Draw points
    glMultiDrawArrays(GL_QUADS, pathOffsets.data(), pathCounts.data(), pathOffsets.size());

    //unbind shader
    lineShader.unbindShader();
}

void PointManager::Draw(RenderDevice *rd, int time, Matrix4 mvp){
    numFramesSeen++;
    clock_t startTime = clock();
    rd->pushState();
    DrawBoxes(rd);
    //printf("Time after setting points: %f\n", ((float)(clock() - startTime)) / CLOCKS_PER_SEC);
    rd->beginOpenGL();
    DrawPoints(time, mvp);
    DrawPaths(time, mvp);
    rd->endOpenGL();
    rd->popState();
    //printf("Time end of frame: %f\n", ((float)(clock() - startTime)) / CLOCKS_PER_SEC);
   }
