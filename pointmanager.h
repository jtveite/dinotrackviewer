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

private:
    std::vector<VRPoint> points;
    VertexBufferRef vb;
    VertexRange vr;
    VertexRange position_range;
    VertexRange color_range;
    void computeLocations(int timesteps);
    void separateVertex(Array<Vertex> array);
    Array<Vector4> posArray;
    Array<Vector4> colArray;
    GLuint buffer;
    GLuint vao;
    MyShader s;
    std::vector< std::vector<Vertex> > pointLocations;
};

#endif // POINTMANAGER_H
