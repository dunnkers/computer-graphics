#include "raytracer.h"

#include "image.h"
#include "light.h"
#include "material.h"
#include "triple.h"

// =============================================================================
// -- Include all your shapes here ---------------------------------------------
// =============================================================================

#include "shapes/sphere.h"
#include "shapes/triangle.h"

// =============================================================================
// -- End of shape includes ----------------------------------------------------
// =============================================================================

#include "json/json.h"

#include <exception>
#include <fstream>
#include <iostream>

using namespace std;        // no std:: required
using json = nlohmann::json;

bool Raytracer::parseObjectNode(json const &node)
{
    ObjectPtr obj = nullptr;

// =============================================================================
// -- Determine type and parse object parametrers ------------------------------
// =============================================================================

    if (node["type"] == "sphere")
    {
        Point pos(node["position"]);
        double radius = node["radius"];
        Vector rotation{};
        int angle = -1;

        if (node.find("rotation") != node.end()) {
            Vector rot(node["rotation"]);
            rotation = rot;
        }

        if (node.find("angle") != node.end()) {
            angle = node["angle"];
        }

		obj = ObjectPtr(new Sphere(pos, radius, rotation, angle));
	}
	else if (node["type"] == "triangle")
	{
		Point vertex0(node["vertex0"]);
		Point vertex1(node["vertex1"]);
		Point vertex2(node["vertex2"]);
		obj = ObjectPtr(new Triangle(vertex0, vertex1, vertex2));
	}
	else
	{
		cerr << "Unknown object type: " << node["type"] << ".\n";
	}

// =============================================================================
// -- End of object reading ----------------------------------------------------
// =============================================================================

    if (!obj)
        return false;

    // Parse material and add object to the scene
    obj->material = parseMaterialNode(node["material"]);
    scene.addObject(obj);
    return true;
}

Light Raytracer::parseLightNode(json const &node) const
{
    Point pos(node["position"]);
    Color col(node["color"]);
    return Light(pos, col);
}

Material Raytracer::parseMaterialNode(json const &node) const
{
    double ka = node["ka"];
    double kd = node["kd"];
    double ks = node["ks"];
    double n  = node["n"];

	if (node.find("color") != node.end()) {
		Color color(node["color"]);
		return Material(color, ka, kd, ks, n);
	}
	
	if (node.find("texture") != node.end()) {
		string const file = node["texture"];
		string append = "../Scenes/";
		append = append + file;
		return Material(append, ka, kd, ks, n);
	}
	
	return Material();
}

bool Raytracer::readScene(string const &ifname)
try
{
    // Read and parse input json file
    ifstream infile(ifname);
    if (!infile) throw runtime_error("Could not open input file for reading.");
    json jsonscene;
    infile >> jsonscene;

// =============================================================================
// -- Read your scene data in this section -------------------------------------
// =============================================================================

    Point eye(jsonscene["Eye"]);
    scene.setEye(eye);

    // TODO: add your other configuration settings here

    if (jsonscene.find("Shadows") != jsonscene.end()) {
        scene.setShadows(jsonscene["Shadows"]);
    }
    
    if (jsonscene.find("MaxRecursionDepth") != jsonscene.end()) {
        scene.setRecursionDepth(jsonscene["MaxRecursionDepth"]);
    }

    if (jsonscene.find("SuperSamplingFactor") != jsonscene.end()) {
        scene.setSamplingFactor(jsonscene["SuperSamplingFactor"]);
    }

    for (auto const &lightNode : jsonscene["Lights"])
        scene.addLight(parseLightNode(lightNode));

    unsigned objCount = 0;
    for (auto const &objectNode : jsonscene["Objects"])
        if (parseObjectNode(objectNode))
            ++objCount;

    cout << "Parsed " << objCount << " objects.\n";

// =============================================================================
// -- End of scene data reading ------------------------------------------------
// =============================================================================

    return true;
}
catch (exception const &ex)
{
    cerr << ex.what() << '\n';
    return false;
}

void Raytracer::renderToFile(string const &ofname)
{
    // TODO: the size may be a settings in your file
    Image img(400, 400);
    cout << "Tracing...\n";
    scene.render(img);
    cout << "Writing image to " << ofname << "...\n";
    img.write_png(ofname);
    cout << "Done.\n";
}
