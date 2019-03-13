#ifndef SCENE_H_
#define SCENE_H_

#include "light.h"
#include "object.h"
#include "triple.h"

#include <vector>

// Forward declarations
class Ray;
class Image;

class Scene
{
    std::vector<ObjectPtr> objects;
    std::vector<LightPtr> lights;   // no ptr needed, but kept for consistency
    Point eye;
    bool shadows = false;
    int recursionDepth = 0;
    int samplingFactor = 1;

    public:

        // trace a ray into the scene and return the color
        Color trace(Ray const &ray, int depth);
        Color reflectRay(Ray ray, int depth, ObjectPtr obj, Hit min_hit);

        void findHitObject(Ray const &ray, ObjectPtr *obj, Hit *min_hit);
        void findHitObject(Ray const &ray, ObjectPtr *obj, Hit *min_hit, 
                            ObjectPtr exclusion);

        // render the scene to the given image
        void render(Image &img);


        void addObject(ObjectPtr obj);
        void addLight(Light const &light);
        void setEye(Triple const& position);

        unsigned getNumObject();
        unsigned getNumLights();

        void setShadows(bool shadows);
        void setRecursionDepth(int depth);
        void setSamplingFactor(int factor);
};

#endif
