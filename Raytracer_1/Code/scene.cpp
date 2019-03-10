#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"

#include <cmath>
#include <limits>

using namespace std;

// Compute Lambertian Shading as in book page 82.
double lambertianShading(double kd, double I, Vector n, Vector l)
{
    double dotProduct = n.dot(l);
    double maxed = fmax(0, dotProduct);
    return kd * I * maxed;
}

// Compute Blinn-Phong Shading as in book page 82/83.
double phongShading(double kd, double I, Vector n, Vector l, 
    Vector h, double ks, double p)
{
    double lambertian = lambertianShading(kd, I, n, l);


    // second term in Phong calculation
    double dotProduct = n.dot(h);
    double maxed = fmax(0, dotProduct);
    double phong = ks * I * pow(maxed, p);

    // add the two terms.
    return lambertian + phong;
}

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

    Color Ia = material.color * material.ka;
    Color Id;
    Color Is;

    for (auto const light : lights) {
        Vector R = 2 * ((light->position - hit).normalized()).dot(N.normalized()) * N.normalized() - (light->position - hit).normalized();
        Id += fmax(0, ((light->position - hit).normalized()).dot(N.normalized())) * material.color * light->color * material.kd;
        Is += pow(fmax(0, R.dot(V)), material.n) * light->color * material.ks;
    }

    Color color = Ia + Id + Is;

    return color;

    // Color color = material.color;
    // double r = color.r;
    // double g = color.g;
    // double b = color.b;
    // double kd = material.kd;
    // double ks = material.ks;
    // double ka = material.ka;

    // // grab light source
    // LightPtr firstLight = lights.front();
    // Light light = *firstLight.get();

    // // compute vector l. subtract intersection point of the ray and surface
    // // from the light source position. book page 82.
    // Vector l = light.position - hit;

    // // Lambertian Shading
    // // color.r = lambertianShading(r * kd, light.color.r, N, l);
    // // color.g = lambertianShading(g * kd, light.color.g, N, l);
    // // color.b = lambertianShading(b * kd, light.color.b, N, l);

    // // vector h is the sum of vectors v and l, normalized. book page 83.
    // Vector H = V + l;
    // H.normalize();

    // l.normalize();

    // // Phong Shading
    // double p = 200.0;
    // color.r = phongShading(r * kd, light.color.r, N, l, H, r * ks, p);
    // color.g = phongShading(g * kd, light.color.g, N, l, H, g * ks, p);
    // color.b = phongShading(b * kd, light.color.b, N, l, H, b * ks, p);

    // // Add Ambient Lighting
    // double intensity = 1.0;
    // color.r = color.r + r * intensity * ka;
    // color.g = color.g + g * intensity * ka;
    // color.b = color.b + b * intensity * ka;

    // return color;
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
