#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "../object.h"

class Triangle: public Object
{
    public:
        Triangle(Point const& vectorA, 
                 Point const& vectorB,
                 Point const& vectorC);

        virtual Hit intersect(Ray const &ray);

        Point const vectorA;
        Point const vectorB;
        Point const vectorC;
};

#endif
