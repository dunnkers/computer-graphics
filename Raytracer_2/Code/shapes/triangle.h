#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "../object.h"

class Triangle: public Object
{
    public:
        Triangle(Point const &v0,
                 Point const &v1,
                 Point const &v2);

        virtual Hit intersect(Ray const &ray);
        virtual Color colorAtTexture(Point N, bool rotate){ return Color(); };
        virtual bool isRotated() { return false; };
        virtual Vector rotate(Point point) { return Vector(); };

        Point v0;
        Point v1;
        Point v2;
        Vector N;
};

#endif
