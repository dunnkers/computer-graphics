#include "triangle.h"

#include <cmath>

using namespace std;

Hit Triangle::intersect(Ray const &ray)
{
    return Hit::NO_HIT();
}

Triangle::Triangle(Point const& vectorA, 
                   Point const& vectorB,
                   Point const& vectorC)
:
    vectorA(vectorA),
    vectorB(vectorB),
    vectorC(vectorC)
{}
