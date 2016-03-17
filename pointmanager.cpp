#include "pointmanager.h"
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
        printf("Reading line %d\r", linecount++);
    }

}

void PointManager::Draw(RenderDevice *rd)
{
    for(auto &point : points){
        point.Draw(rd,0);
    }
}
