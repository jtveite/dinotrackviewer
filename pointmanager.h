#ifndef POINTMANAGER_H
#define POINTMANAGER_H
#include <vector>
#include <string>
#include "vrpoint.h"
#include <vrg3d/VRG3D.h>
#include "vertex.h"
#include "shader.h"
#include "filter.h"
#include <GL/gl.h>
#include <glm/glm.hpp>


using namespace G3D;


class PointManager
{
public:
    PointManager();
    void ReadFile(std::string fileName, bool debug = false);
    void SetupDraw(bool allPaths = false);
    void ReadPathlines(std::string fileName);
    void Draw(RenderDevice *rd, int time, glm::mat4 mvp);
    void AddPathline(glm::vec3 pos, int time);
    void AddPathline(VRPoint& point);
    void TempPathline(glm::vec3 pos, int time);
    void SetFilter(Filter* f);
    float pointSize = 0.0004;
    int getLength();
    std::vector<VRPoint> points;

private:
    void DrawBoxes(RenderDevice *rd);
    void DrawPoints(int time, glm::mat4 mvp);
    void DrawPaths(int time, glm::mat4 mvp);
   
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

    std::vector<AABox> boxes;


    std::vector<int> pathlines;
    void computeLocations();
    GLuint buffer;
    GLuint pathBuffer;
    GLuint tempPathBuffer;
    bool useSeparateBuffers = true;
    GLuint* pointBuffers;
    GLuint vao;
    MyShader pointShader;
    MyShader lineShader;
    std::vector< std::vector<Vertex> > pointLocations;
    Image3::Ref colorTexture;
    Image3::Ref pathTexture;

    glm::vec3 minV;
    glm::vec3 maxV;

    void retestVisible();
};

#endif // POINTMANAGER_H
