#include "pointmanager.h"
#include "vrpoint.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

PointManager::PointManager()
{

}

void PointManager::ReadFile(std::string fileName)
{
    std::fstream file;
    std::string line;
    file.open(fileName, std::ios::in);
    int linecount = 0;
    while(std::getline(file, line))
    {
        std::istringstream iss(line);
        int id;
        float x,y,z;
        iss >> id;
        VRPoint p (id);
        while (iss >> x >> y >> z){
            p.AddPoint(Vector3(x,y,z));
        }
        points.push_back(p);
    //    printf("Reading line %d\r", linecount++);
    }

}

void PointManager::Draw(RenderDevice *rd, int time)
{
    rd->pushState();
    Matrix3 scale = Matrix3::fromDiagonal(Vector3(20, 20, 20));
    rd->setObjectToWorldMatrix(CoordinateFrame(scale) * rd->objectToWorldMatrix());
    for(auto &point : points){
    //    point.Draw(rd,time);
    }
    //points[0].Draw(rd, time);

    rd->beginPrimitive(PrimitiveType::POINTS);
    for(int i = 0; i < points.size(); i++){
        VRPoint point = points[i];
        point.Draw(rd, time);
    }
    rd->endPrimitive();
    rd->popState();
}
