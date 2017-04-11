#ifndef POINTMANAGER_H
#define POINTMANAGER_H

#if defined(__APPLE__)
#include <GL/glew.h>
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif


#include "GLUtil.h"
#include <vector>
#include <string>
#include "vrpoint.h"
#include "vertex.h"
#include "filter.h"
#include "shader.h"
#include "glm/glm.hpp"
#include "movm.h"



class PointManager
{
public:
    PointManager();
    void ReadFile(std::string fileName, bool debug = false);
    void ReadMoVMFs(std::string fileName);
    void SetupDraw(bool allPaths = false);
    void ReadPathlines(std::string fileName);
    void Draw(int time, glm::mat4 mvp);
    void AddPathline(glm::vec3 pos, int time);
    void AddPathline(VRPoint& point);
    void ClearPathlines();
    void TempPathline(glm::vec3 pos, int time);
    void SetFilter(Filter* f);
    float pointSize = 0.0004;
    int getLength();
    std::vector<VRPoint> points;

    void AddSeedPoint(glm::vec3 pos, int time);
    void SearchForSeeds(int target_count = 10);
    void ResetPrediction();

    bool clustering = true;
    int currentCluster = -1;
    bool clustersChanged = true;
    void ShowCluster(glm::vec3 pos, int time);
    void ReadClusters(std::string fileName);

    std::unordered_map<int, MoVMF> movmfs;
    MoVMF seeds;
    int seedCount;
    bool seedsChanged = true;
    int seedSearches = 0;
    std::vector<std::pair<double, int>> scores;

    std::vector< std::vector<int> > clusters;

    bool drawAllClusters = false;
    bool firstTimeDrawingAll = true;
    GLuint* megaClusterMegaBuffers;
    GLuint megaClustersBuffer;
    std::vector<int> stupidVertexCount;
    int bufferVertexCount = 0;

    float pathlineMin = 0.0;
    float pathlineMax = 1.0;
private:
    void DrawBoxes();
    void DrawPoints(int time, glm::mat4 mvp);
    void DrawPaths(int time, glm::mat4 mvp);
    void DrawClusters(int time, glm::mat4 mvp);
    void DrawAllClusters(int time, glm::mat4 mvp);
  
    int FindPointIndex(int pointID);

    //Returns the index of the pathline closest to the given point
    int FindPathline(glm::vec3 pos, int time, float min = 10000.f);


    std::vector<int> visiblePoints;

    Filter* filter;

    int numFramesSeen;

    int timeSteps;
    
    std::vector<int> pathOffsets;
    std::vector<int> pathCounts;
    std::vector<Vertex> pathVertices;
    std::vector<Vertex> tempPath;
    bool updatePaths;

//    std::vector<AABox> boxes;


    std::vector<int> pathlines;
    void computeLocations();
    GLuint buffer;
    GLuint pathBuffer;
    GLuint tempPathBuffer;
    GLuint clusterBuffer;
    GLuint megaClusterBuffer;
    int clusterVertCount;
    bool useSeparateBuffers = true;
    GLuint* pointBuffers;
    GLuint vao;

    MyShader* pointShader;
    MyShader* lineShader;

    std::vector< std::vector<Vertex> > pointLocations;

    glm::vec3 minV;
    glm::vec3 maxV;

    void retestVisible();
};

#endif // POINTMANAGER_H
