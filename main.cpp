#include <iostream>
#include "pointmanager.h"

using namespace std;

int main(int argc, char *argv[])
{
    cout << "Hello World!" << endl;
    PointManager p;
    cout << argv[1] << endl;
    p.ReadFile(argv[1]);
    p.Draw(NULL);
    return 0;
}
