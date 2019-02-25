#include "sphere.h"

#include <cmath>

using namespace std;

Hit Sphere::intersect(Ray const &ray)
{
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    *
    * Insert calculation of ray/sphere intersection here.
    *
    * You have the sphere's center (C) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return false.
    * Otherwise, return true and place the distance of the
    * intersection point from the ray origin in *t (see example).
    ****************************************************/

    // Intersection calculation
    Point origin = ray.O;
    Vector direction = ray.D;
    double radius = r;

    // solves the quadratic equation At^2 + Bt + C = 0 (book page 77)
    Point oc = origin - position;
    float a = direction.dot(direction);
    float b = 2.0 * oc.dot(direction);
    float c = oc.dot(oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    // if (discriminant < 0) {
    //     return Hit::NO_HIT();
    // }

    if (discriminant > 0) {
        double t = (-b - sqrt(discriminant)) / (2.0 * a);
        Point center = position;
        Vector surfacePoint = ray.O + t * ray.D;
        Vector N = surfacePoint - center;
        N.normalize();
        return Hit(t, N);
    }

    Vector OC = (position - ray.O).normalized();
    if (OC.dot(ray.D) < 0.999) {
        return Hit::NO_HIT();
    }


    /****************************************************
    * RT1.2: NORMAL CALCULATION
    *
    * Given: t, C, r
    * Sought: N
    *
    * Insert calculation of the sphere's normal at the intersection point.
    ****************************************************/
}

Sphere::Sphere(Point const &pos, double radius)
:
    position(pos),
    r(radius)
{}
