#include <vrg3d/VRG3D.h>
#include <vector>

#include "vertex.h"

#ifndef VRPOINT_H
#define VRPOINT_H

using namespace G3D;

class VRPoint {

public:
    VRPoint(int id);
    Vertex Draw(int time, Image3::Ref image);
    void AddPoint(Vector3 point);
    Vector4 GetColor(int time, Image3::Ref image);

    void DrawPathline(RenderDevice *rd);

    float GetDistance(int time, Vector3 point);

    std::vector<Vertex> getPathlineVerts(Image3::Ref tex);
private:
    Vector4 GetColorHorizontalPosition();
    std::vector<Vector3> positions;
    int m_id;

};
#endif // VRPOINT_H
