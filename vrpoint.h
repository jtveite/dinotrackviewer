#include <vector>
#include <glm/glm.hpp>


#include "vertex.h"

#ifndef VRPOINT_H
#define VRPOINT_H


class VRPoint {

public:
    VRPoint(int id);
    Vertex Draw(int time, glm::vec3 minV, glm::vec3 maxV);
    void AddPoint(glm::vec3 point);
    glm::vec2 GetColor(int time, glm::vec3 minV, glm::vec3 maxV);


    float GetDistance(int time, glm::vec3 point);

    float totalPathLength();

    bool withinDistance(VRPoint& other, double distance);

    std::vector<Vertex> getPathlineVerts();
    std::vector<glm::vec3> positions;

    int steps();
    int m_id;
private:
    glm::vec2 getColor(int);
    glm::vec4 GetColorHorizontalPosition();

};
#endif // VRPOINT_H
