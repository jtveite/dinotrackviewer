#include "dummy.h"
#include <vector>

#ifndef VRPOINT_H
#define VRPOINT_H
class VRPoint {

public:
    VRPoint(int id);
    void Draw(RenderDevice *rd, int time);
    void AddPoint(Vector3 point);
private:
    std::vector<Vector3> positions;
    int m_id;

};
#endif // VRPOINT_H
