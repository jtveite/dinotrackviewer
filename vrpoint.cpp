#include "vrpoint.h"
VRPoint::VRPoint(int id)
{
    m_id = id;
}

void VRPoint::Draw(RenderDevice *rd, int time)
{
    if (time > positions.size()){
        return;
        //Fail silently
    }
    printf("Point is at %.3f, %.3f, %.3f.\n", positions[time].x, positions[time].y, positions[time].z);



}

void VRPoint::AddPoint(Vector3 point)
{
    positions.push_back(point);
}
