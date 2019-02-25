#include "triangle.h"

#include <cmath>

using namespace std;

Hit Triangle::intersect(Ray const &ray)
{
    return Hit::NO_HIT();
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    ****************************************************/



    //    @SEE PAGE 79 BOOK



    // // Intersection calculation
    // Point origin = ray.O;
    // Vector direction = ray.D;
    // double radius = r;

    // // solves the quadratic equation At^2 + Bt + C = 0 (book page 77), using abc-method.
    // Point oc = origin - position;
    // float a = direction.dot(direction);
    // float b = 2.0 * oc.dot(direction);
    // float c = oc.dot(oc) - radius * radius;
    // float discriminant = b * b - 4 * a * c;

    // // there are solutions for discriminant geq 0.
    // if (discriminant > 0) {
    //     double t = (-b - sqrt(discriminant)) / (2.0 * a);

    //     /****************************************************
    //     * RT1.2: NORMAL CALCULATION
    //     ****************************************************/
    //     Point center = position;
    //     Vector surfacePoint = ray.O + t * ray.D;
    //     Vector N = surfacePoint - center;
    //     N.normalize();
        
    //     // return a Hit with newly calculated variables 
    //     return Hit(t, N);
    // } else { // discriminant less than 0, no hit.
    //     return Hit::NO_HIT();
    // }
}

Triangle::Triangle(Point const& vectorA, 
                   Point const& vectorB,
                   Point const& vectorC)
:
    vectorA(vectorA),
    vectorB(vectorB),
    vectorC(vectorC)
{}
