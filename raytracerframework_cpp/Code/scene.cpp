#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"

#include <cmath>
#include <limits>

using namespace std;

Color Scene::trace(Ray const &ray)
{
    // Find hit object and distance
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj = nullptr;
    for (unsigned idx = 0; idx != objects.size(); ++idx)
    {
        Hit hit(objects[idx]->intersect(ray));
        if (hit.t < min_hit.t)
        {
            min_hit = hit;
            obj = objects[idx];
        }
    }

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);

    Material material = obj->material;          //the hit objects material
    Point hit = ray.at(min_hit.t);                 //the hit point
    Vector N = min_hit.N;                          //the normal at hit point
    Vector V = -ray.D;                             //the view vector

    /****************************************************
    * This is where you should insert the color
    * calculation (Phong model).
    *
    * Given: material, hit, N, V, lights[]
    * Sought: color
    *
    * Hints: (see triple.h)
    *        Triple.dot(Vector) dot product
    *        Vector + Vector    vector sum
    *        Vector - Vector    vector difference
    *        Point - Point      yields vector
    *        Vector.normalize() normalizes vector, returns length
    *        double * Color     scales each color component (r,g,b)
    *        Color * Color      dito
    *        pow(a,b)           a to the power of b
    ****************************************************/

    Color color = material.color;                  // place holder
    double r = color.r;
    double g = color.g;
    double b = color.b;
    double kd = material.kd;
    double ks = material.ks;

    // grab light source
    LightPtr light = lights.front();
    Light reallight = *light.get();
    // compute vector l. subtract intersection point of the ray and surface
    // from the light source position. book page 82.
    Vector l = reallight.position - hit;
    // vector h is the sum of vectors v and l, normalized. book page 83.
    Vector H = V + l;
    H.normalize();

    double dotProduct = N.dot(l);

    if (dotProduct > 0 ) {
        double dotProductnh = N.dot(H);

        if ((N.dot(H)) > 0 ) {
            color.r = r * kd * dotProduct + r * dotProductnh * ks;
            color.g = g * kd * dotProduct + g * dotProductnh * ks;
            color.b = b * kd * dotProduct + b * dotProductnh * ks;
        } else {
            color.r = r * kd * dotProduct;
            color.g = g * kd * dotProduct;
            color.b = b * kd * dotProduct;
        }
        
    } else {
        color.r = 0;
        color.g = 0;
        color.b = 0;
    }
    
    return color;
}

void Scene::render(Image &img)
{
    unsigned w = img.width();
    unsigned h = img.height();
    for (unsigned y = 0; y < h; ++y)
    {
        for (unsigned x = 0; x < w; ++x)
        {
            Point pixel(x + 0.5, h - 1 - y + 0.5, 0);
            Ray ray(eye, (pixel - eye).normalized());
            Color col = trace(ray);
            col.clamp();
            img(x, y) = col;
        }
    }
}

// --- Misc functions ----------------------------------------------------------

void Scene::addObject(ObjectPtr obj)
{
    objects.push_back(obj);
}

void Scene::addLight(Light const &light)
{
    lights.push_back(LightPtr(new Light(light)));
}

void Scene::setEye(Triple const &position)
{
    eye = position;
}

unsigned Scene::getNumObject()
{
    return objects.size();
}

unsigned Scene::getNumLights()
{
    return lights.size();
}
