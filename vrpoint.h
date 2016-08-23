#include <vrg3d/VRG3D.h>
#include <vector>

#include "vertex.h"

#ifndef VRPOINT_H
#define VRPOINT_H

using namespace G3D;

class VRPoint {

public:
    VRPoint(int id);
    Vertex Draw(int time, Vector3 minV, Vector3 maxV);
    void AddPoint(Vector3 point);
    Vector2 GetColor(int time, Vector3 minV, Vector3 maxV);

    void DrawPathline(RenderDevice *rd);

    float GetDistance(int time, Vector3 point);

    float totalPathLength();

    bool withinDistance(VRPoint& other, double distance);

    std::vector<Vertex> getPathlineVerts();
    std::vector<Vector3> positions;

    int steps();
    int m_id;
private:
    Vector2 getColor(int);
    Vector4 GetColorHorizontalPosition();

};
#endif // VRPOINT_H
