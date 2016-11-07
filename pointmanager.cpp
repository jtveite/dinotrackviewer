#include "pointmanager.h"
#include "vrpoint.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>

#ifdef PROFILING
#include "gperftools/profiler.h"
#endif

#include "fast_atof.c"

PointManager::PointManager()
{

}

void PointManager::ReadFile(std::string fileName, bool debug)
{
    
    if (debug){
      std::cout << "Reading file: " << fileName << std::endl;
    }

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
          glm::vec3 lower(x,y,z);
          iss >> x >> y >> z;
          glm::vec3 upper(x,y,z);
          // REDO THE BOXES
          //AABox box (lower, upper);//think about readding checks for lower/upper
          //printf("Read box %f,%f,%f:%f,%f,%f\n", lower.x, lower.y, lower.z, upper.x, upper.y, upper.z );
          //boxes.push_back(box);
          continue;
        }
        iss >> id;
        VRPoint p (id);
        /*
        while (iss >> x >> y >> z){
            p.AddPoint(glm::vec3(x,y,z));
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
            p.AddPoint(glm::vec3(arr[0], arr[1], arr[2]));
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
      minV = glm::min(minV, points[i].positions[0]);
      maxV = glm::max(maxV, points[i].positions[0]);
    }
    


}

void PointManager::SetupDraw(bool allPaths){
    clock_t startTime = clock();
  


    filter = new Filter();
   
    if (useSeparateBuffers && timeSteps > 0){
      pointBuffers = new GLuint[timeSteps];
      glGenBuffers(timeSteps, pointBuffers);
    }
   
   
    retestVisible();
  

//Why does it crash on CCV systems if there's a vao? I haven't a clue. But it does.
#if defined(__APPLE__)
    GLuint vao;
    GLuint tempVar;
    glGenVertexArrays(1, &tempVar);
    glBindVertexArray(tempVar);
#endif

    glGenBuffers(1, &buffer);
    glGenBuffers(1, &pathBuffer);
    glGenBuffers(1, &tempPathBuffer);
    pointShader = new MyShader("shaders/basic.vert", "shaders/basic.geom", "shaders/basic.frag");
    pointShader->checkErrors();
    pointShader->loadTexture("colorMap", "colormap.jpg");
    lineShader = new MyShader("shaders/path.vert", "shaders/path.frag");
    lineShader->checkErrors();
    lineShader->loadTexture("pathMap", "pathmap.jpg");


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

  if(useSeparateBuffers){
    for(int i = 0; i < timeSteps; i++){
      glBindBuffer(GL_ARRAY_BUFFER, pointBuffers[i]);
      std::vector<Vertex> *pointArray = &pointLocations[i];
      int bufferSize = pointArray->size() * sizeof(Vertex);
      Vertex* bufferData = pointArray->data();
      glBufferData(GL_ARRAY_BUFFER, bufferSize, bufferData, GL_STATIC_DRAW);

    }
  }
}

int PointManager::FindPathline(glm::vec3 pos, int time, float minDist){
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
  return bestID;
} 

void PointManager::TempPathline(glm::vec3 pos, int time){
  int bestID = FindPathline(pos, time);
  VRPoint& point = points[bestID];
  tempPath = point.getPathlineVerts();
}

void PointManager::AddPathline(glm::vec3 pos, int time){
  int bestID = FindPathline(pos, time); 
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


void PointManager::DrawBoxes(){
  return;
/*  for(int i = 0; i < boxes.size(); i++){
    //AABox box = boxes[i];
    float gray = 0.4;
    //Draw::box(box, rd, Color4::clear(), Color4(gray, gray, gray, 1.));
  }*/
}

void PointManager::DrawPoints(int time, glm::mat4 mvp){
    int err;
    glCheckError();
    //Bind shader and set args
    pointShader->bindShader();
    glCheckError();
    pointShader->setMatrix4("mvp", mvp);
    pointShader->setFloat("rad", pointSize);
    glCheckError();
    //Bind and resend buffer data
    int numElements;
    if (useSeparateBuffers){
      if (time < timeSteps){
        glBindBuffer(GL_ARRAY_BUFFER, pointBuffers[time]);
        numElements = pointLocations[time].size();
      }
      else{
        return;
      }
    }
    else{
      glBindBuffer(GL_ARRAY_BUFFER, buffer);
      std::vector<Vertex> *pointArray = &pointLocations[time];
      numElements = pointArray->size();
      int bufferSize = pointArray->size() * sizeof(Vertex);
      Vertex* bufferData = pointArray->data();
      glBufferData(GL_ARRAY_BUFFER, bufferSize, bufferData, GL_DYNAMIC_DRAW);
    }
    glCheckError();

    //set vertex attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glCheckError();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex), NULL);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) sizeof(glm::vec3));
    
    glCheckError();

    //Draw the points
    glDrawArrays(GL_POINTS, 0, numElements);
    glCheckError();
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Unbind shader
    pointShader->unbindShader();
}

 
void PointManager::DrawPaths(int time, glm::mat4 mvp){
    int err;
    glCheckError();
    //Bind shader and set args
    lineShader->bindShader();
    lineShader->setMatrix4("mvp", mvp);
    glCheckError();

    //Bind buffer, resend data if needed
    glBindBuffer(GL_ARRAY_BUFFER, pathBuffer);
    //updatePaths ensures that we only write new paths to the gpu when needed
    int bufferSize = pathVertices.size() * sizeof(Vertex);
    if (updatePaths){
      updatePaths = false;
      Vertex* bufferData = pathVertices.data();
      glBufferData(GL_ARRAY_BUFFER, bufferSize, bufferData, GL_DYNAMIC_DRAW);
    }
    glCheckError();

    //set vertex attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex), NULL);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) sizeof(glm::vec3));
    glCheckError();
    //Draw points
    glDrawArrays(GL_TRIANGLES,  0, bufferSize);
    glCheckError();


    //Draw temporary paths
    if (tempPath.size() > 0){
      glBindBuffer(GL_ARRAY_BUFFER, tempPathBuffer);
      int bufferSize = tempPath.size() * sizeof(Vertex);
      Vertex* bufferData = tempPath.data();
      glBufferData(GL_ARRAY_BUFFER, bufferSize, bufferData, GL_DYNAMIC_DRAW);
      
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex), NULL);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) sizeof(glm::vec3));


      glDrawArrays(GL_TRIANGLES, 0, tempPath.size());
    }
    glCheckError();
    //unbind shader
    lineShader->unbindShader();
}

void PointManager::Draw(int time, glm::mat4 mvp){
    numFramesSeen++;
    clock_t startTime = clock();
    int err;
    glCheckError();
    DrawBoxes();
    //printf("Time after setting points: %f\n", ((float)(clock() - startTime)) / CLOCKS_PER_SEC);
    glCheckError();
    DrawPoints(time, mvp);
    glCheckError();
    DrawPaths(time, mvp);
    glCheckError();
    //printf("Time end of frame: %f\n", ((float)(clock() - startTime)) / CLOCKS_PER_SEC);
   }
