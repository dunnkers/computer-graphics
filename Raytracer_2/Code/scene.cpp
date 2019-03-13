#include "scene.h"

#include "hit.h"
#include "image.h"
#include "material.h"
#include "ray.h"

#include <cmath>
#include <limits>

using namespace std;

void Scene::findHitObject(Ray const &ray, ObjectPtr *obj, Hit *min_hit)
{
    findHitObject(ray, obj, min_hit, nullptr);
}

void Scene::findHitObject(Ray const &ray, ObjectPtr *obj, Hit *min_hit, 
    ObjectPtr exclusion)
{
    for (unsigned idx = 0; idx != objects.size(); ++idx) {
        Hit hit(objects[idx]->intersect(ray));
        if (hit.t < min_hit->t && objects[idx] != exclusion) {
            *min_hit = hit;
            *obj = objects[idx];
        }
    }
}

Color Scene::trace(Ray const &ray, int currentDepth)
{
    // Find hit object and distance
    Hit min_hit(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj = nullptr;
    findHitObject(ray, &obj, &min_hit);

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);

    Material &material = obj->material;        //the hit objects material
    Point hit = ray.at(min_hit.t - 1e-15);      //the hit point
    Vector V = -ray.D;
    Vector N = min_hit.N;

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

    if (material.isTextured()) {
        material.color = obj->textureColorAt(hit, obj->isRotated());
    }

    // Ia is constant, other terms not
    Color Ia = material.color * material.ka;
    Color Is;
    Color Id;

    for (auto const light : lights) {
        // book pg 82
        Vector l = light->position - hit;
        l.normalize();
        N.normalize();

        // shadows obj that is potentially hit
        ObjectPtr obj_shad = nullptr;

        if (shadows) {
            Ray ray_shad(light->position, -l);
            Hit min_hit_shad(numeric_limits<double>::infinity(), Vector());
            findHitObject(ray_shad, &obj_shad, &min_hit_shad);
        }

        if (!shadows || (shadows && obj_shad == obj)) {
            // book pg 238
            Vector r = -l + 2 * l.dot(N) * N;
            // Is - Specular reflection (lecture slides)
            // material.n resembles Phong specular component p
            Is += pow(fmax(0, r.dot(V)), material.n) * material.ks * light->color;
            // Id - Diffuse term - Lambert's law (lecture slides)
            Id += fmax(0, N.dot(l)) * material.color * material.kd * light->color;

            if (currentDepth < recursionDepth) {
                Is += traceRefl(ray, currentDepth, obj, min_hit);
            }
        }
    }

    // add up all terms
    Color I = Ia + Is + Id;
    return I;
}

Color Scene::traceRefl(Ray ray, int depth, ObjectPtr obj, Hit min_hit)
{
    // calc hitpoint
    Point hit = ray.at(min_hit.t - 1e-15); //the hit point
    Vector N = min_hit.N;
    Vector r = (N * 2 * (N.dot(-ray.D)) + ray.D).normalized();

    // new ray
    Ray ray_refl{ hit, r };
    // Find hit object and distance
    Hit min_hit_reflected(numeric_limits<double>::infinity(), Vector());
    ObjectPtr obj_hit_refl = nullptr;
    findHitObject(ray_refl, &obj_hit_refl, &min_hit_reflected, obj);

    Material& material = obj->material;
    Vector V = -ray.D;

    // Return background color.if no object reflected hit
    if (!obj_hit_refl) return Color(0.0, 0.0, 0.0);

    Point hit_refl = ray_refl.at(min_hit_reflected.t - 1e-15);
    
    // recurse into another trace
    Light light_refl(hit_refl, trace(ray_refl, depth + 1) * material.ks);
    Vector L = (light_refl.position - hit).normalized();
    r = N * 2 * (N.dot(L)) - L;

    Color Is;

    Is += pow(fmax(0, r.dot(V)), material.n) * material.ks * light_refl.color;
    
    // add up all terms
    Color I =  Is;
    return I;
}

void Scene::render(Image &img)
{
    unsigned w = img.width();
    unsigned h = img.height();
    for (float i = 0.5 / samplingFactor; i < h; i += 1.0 / samplingFactor) {
        for (float j = 0.5 / samplingFactor; j < w; j += 1.0 / samplingFactor) {
            Point pixel(0.5 + i, 0.5 + (h - j - 1), 0);
            Ray ray(eye, (pixel - eye).normalized());
            Color col = trace(ray, 0);
            col.clamp();
            
            int width = (int) i; // cast float to int
            int height = (int) j; // cast float to int
            img(width, height) += col / (samplingFactor * samplingFactor);
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

void Scene::setShadows(bool shad) {
    shadows = shad;
}

void Scene::setRecursionDepth(int depth) {
    recursionDepth = depth;
}

void Scene::setSamplingFactor(int factor)
{
    samplingFactor = factor;
}