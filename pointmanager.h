#ifndef POINTMANAGER_H
#define POINTMANAGER_H
#include <vector>
#include <string>
#include "vrpoint.h"
#include <vrg3d/VRG3D.h>
#include "vertex.h"
#include "shader.h"
#include <GL/gl.h>

using namespace G3D;


class PointManager
{
public:
    PointManager();
    void ReadFile(std::string fileName);
    void Draw(RenderDevice *rd, int time, Matrix4 mvp);
    void AddPathline(Vector3 pos, int time);
    float pointSize = 0.0004;

private:
    std::vector<VRPoint> points;
    int numFramesSeen;
    
    std::vector<int> pathOffsets;
    std::vector<int> pathCounts;
    std::vector<Vertex> pathVertices;
    bool updatePaths;


    std::vector<int> pathlines;
    void computeLocations(int timesteps);
    GLuint buffer;
    GLuint pathBuffer;
    GLuint vao;
    MyShader pointShader;
    MyShader lineShader;
    std::vector< std::vector<Vertex> > pointLocations;
    Image3::Ref colorTexture;
    Image3::Ref pathTexture;
};

#endif // POINTMANAGER_H
