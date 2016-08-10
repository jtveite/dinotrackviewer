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

using namespace G3D;


class PointManager
{
public:
    PointManager();
    void ReadFile(std::string fileName, bool debug = false);
    void SetupDraw(bool allPaths = false);
    void ReadPathlines(std::string fileName);
    void Draw(RenderDevice *rd, int time, Matrix4 mvp);
    void AddPathline(Vector3 pos, int time);
    void AddPathline(VRPoint& point);
    void SetFilter(Filter* f);
    float pointSize = 0.0004;
    int getLength();
    std::vector<VRPoint> points;

private:
    void DrawBoxes(RenderDevice *rd);
    void DrawPoints(int time, Matrix4 mvp);
    void DrawPaths(int time, Matrix4 mvp);

    
  

    std::vector<int> visiblePoints;

    Filter* filter;

    int numFramesSeen;

    int timeSteps;
    
    std::vector<int> pathOffsets;
    std::vector<int> pathCounts;
    std::vector<Vertex> pathVertices;
    bool updatePaths;

    std::vector<AABox> boxes;


    std::vector<int> pathlines;
    void computeLocations();
    GLuint buffer;
    GLuint pathBuffer;
    GLuint vao;
    MyShader pointShader;
    MyShader lineShader;
    std::vector< std::vector<Vertex> > pointLocations;
    Image3::Ref colorTexture;
    Image3::Ref pathTexture;

    void retestVisible();
};

#endif // POINTMANAGER_H
