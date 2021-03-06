//
//  Framework for a raytracer
//  File: raytracer.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Author: Maarten Everts
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#include "raytracer.h"
#include "object.h"
#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "material.h"
#include "light.h"
#include "image.h"
#include "yaml/yaml.h"
#include "scene.h"
#include <ctype.h>
#include <fstream>
#include <assert.h>

// Functions to ease reading from YAML input
void operator >> (const YAML::Node& node, Triple& t);
Triple parseTriple(const YAML::Node& node);

void operator >> (const YAML::Node& node, Triple& t)
{
    assert(node.size()==3);
    node[0] >> t.x;
    node[1] >> t.y;
    node[2] >> t.z;
}

Triple parseTriple(const YAML::Node& node)
{
    Triple t;
    node[0] >> t.x;
    node[1] >> t.y;
    node[2] >> t.z;	
    return t;
}

Material* Raytracer::parseMaterial(const YAML::Node& node)
{
    Material *m = new Material();
    node["color"] >> m->color;	
    node["ka"] >> m->ka;
    node["kd"] >> m->kd;
    node["ks"] >> m->ks;
    node["n"] >> m->n;
    return m;
}

Object* Raytracer::parseObject(const YAML::Node& node)
{
    Object *returnObject = NULL;
    std::string objectType;
    node["type"] >> objectType;

    if (objectType == "sphere") {
        Point pos;
        node["position"] >> pos;
        double r;
        node["radius"] >> r;
        Sphere *sphere = new Sphere(pos,r);		
        returnObject = sphere;
    }
    if (objectType == "triangle"){ // added object Triagle, which is 3 points
        Point position0, position1, position2;
        node["position0"] >> position0;
        node["position1"] >> position1;
        node["position2"] >> position2;
        Triangle *triangle = new Triangle(position0, position1, position2);
        returnObject = triangle;
    }
    if(objectType == "plane"){ // added object (finite) plane, which is 4 points. 
        Point position0, position1, position2, position3;
        double minX, maxX, minY, maxY, minZ, maxZ;
        node["position0"] >> position0;
        node["position1"] >> position1;
        node["position2"] >> position2;
        node["position3"] >> position3;
        
        // Max and Min x/y/z positions are calculated and also given to the Plane
        Point array[4] = {position0, position1, position2, position3};
		minX = minY = minZ = numeric_limits<double>::max();
		maxX = maxY = maxZ = -numeric_limits<double>::max();
		for(unsigned int i = 0; i < 4 ; i++){
			if(array[i].x < minX) minX = array[i].x;
			if(array[i].y < minY) minY = array[i].y;
			if(array[i].z < minZ) minZ = array[i].z;
		
			if(array[i].x > maxX) maxX = array[i].x;
			if(array[i].y > maxY) maxY = array[i].y;
			if(array[i].z > maxZ) maxZ = array[i].z;
		}
        
        Plane *plane = new Plane(position0, position1, position2, position3,minX, maxX, minY, maxY, minZ, maxZ);
        returnObject = plane;
    }
    if (returnObject) {
        // read the material and attach to object
        returnObject->material = parseMaterial(node["material"]);
    }

    return returnObject;
}

Light* Raytracer::parseLight(const YAML::Node& node)
{
    Point position;
    node["position"] >> position;
    Color color;
    node["color"] >> color;
    return new Light(position,color);
}

//Parse given supersampling number
void Raytracer::parseSuperSampling(const YAML::Node& node){
    int i;
    node["factor"] >> i;
    scene->setSuperSampling(i);
}

void Raytracer::parseCamera(const YAML::Node& node){
    Triple eye, center, up;
    //int size[2];
    node["eye"] >> eye;
    node["center"] >> center;
    node["up"] >> up;
    //set size, can't parse a triple (two variables are only given)
    size = Triple(node["viewSize"][0], node["viewSize"][1],0) ;
    //Set camera in scene
    scene->useCamera(true);
    scene->setCamera(eye, center, up);
}
/*
* Read a scene from file
*/

bool Raytracer::readScene(const std::string& inputFilename)
{
    // Initialize a new scene
    scene = new Scene();

    // Open file stream for reading and have the YAML module parse it
    std::ifstream fin(inputFilename.c_str());
    if (!fin) {
        cerr << "Error: unable to open " << inputFilename << " for reading." << endl;;
        return false;
    }
    try {
        YAML::Parser parser(fin);
        if (parser) {
            YAML::Node doc;
            parser.GetNextDocument(doc);
            // read how the file should be processed
			mode = 0;
			if(const YAML::Node* render = doc.FindValue("RenderMode")){
				string m;
				*render >> m;
				if (m.compare("normal") == 0){
					mode = 1;
				} else if(m.compare("zbuffer") == 0) {
					mode = 2;
				}
			}
            
            //read camera variables if given
            if(const YAML::Node* camera = doc.FindValue("Camera")){
                parseCamera(*camera);
            }else{
                scene->useCamera(false);
            }

			// Check if shadows need to be rendered, default is false (no shadows)
			scene->setRenderShadows(false);
			if(const YAML::Node* shadows = doc.FindValue("Shadows")){
				string s;
				*shadows >> s;
				if (s.compare("true") == 0){
					scene->setRenderShadows(true);
				}
			}
			
			// Check if there is a Max recursion depth, default is 1 (no Recursion)
			scene->setMaxRecursionDepth(1);
			if(const YAML::Node* maxRecDepth = doc.FindValue("MaxRecursionDepth")){
				int i;
				*maxRecDepth >> i;
				scene->setMaxRecursionDepth(i);
			}
			
            // Check if there is a Max recursion depth, default is 1 (no SS)
            scene->setSuperSampling(1);
            if(const YAML::Node* supSample = doc.FindValue("SuperSampling")){
                parseSuperSampling(*supSample);
            }
			
            // Read scene configuration options
            if(const YAML::Node* eye = doc.FindValue("Eye")){
                Triple eyeTrip;
                *eye >> eyeTrip;
                scene->setEye(eyeTrip);
            }

            // Read and parse the scene objects
            const YAML::Node& sceneObjects = doc["Objects"];
            if (sceneObjects.GetType() != YAML::CT_SEQUENCE) {
                cerr << "Error: expected a sequence of objects." << endl;
                return false;
            }
            for(YAML::Iterator it=sceneObjects.begin();it!=sceneObjects.end();++it) {
                Object *obj = parseObject(*it);
                // Only add object if it is recognized
                if (obj) {
                    scene->addObject(obj);
                } else {
                    cerr << "Warning: found object of unknown type, ignored." << endl;
                }
            }

            // Read and parse light definitions
            const YAML::Node& sceneLights = doc["Lights"];
            if (sceneObjects.GetType() != YAML::CT_SEQUENCE) {
                cerr << "Error: expected a sequence of lights." << endl;
                return false;
            }
            for(YAML::Iterator it=sceneLights.begin();it!=sceneLights.end();++it) {
                scene->addLight(parseLight(*it));
            }
        }
        if (parser) {
            cerr << "Warning: unexpected YAML document, ignored." << endl;
        }
    } catch(YAML::ParserException& e) {
        std::cerr << "Error at line " << e.mark.line + 1 << ", col " << e.mark.column + 1 << ": " << e.msg << std::endl;
        return false;
    }

    cout << "YAML parsing results: " << scene->getNumObjects() << " objects read." << endl;
    return true;
}

void Raytracer::renderToFile(const std::string& outputFilename)
{
    // Imgae is now pointer
    Image *img;
    if (scene->getUseCamera() == true) {
        //If camera is used, create new image using size
        img = new Image(size.x, size.y);
    }else{
        //If no camera is used, set default image (400,400)
        img = new Image(400,400);
    }
    cout << "Tracing..." << endl;
    // Depending on the file a different render is chosen
    if(mode ==0){
        scene->render(*img);
    }else if(mode == 1){
        scene->renderNormal(*img);
    }else {
        scene->renderZ(*img);
    }
    cout << "Writing image to " << outputFilename << "..." << endl;
    img->write_png(outputFilename.c_str());
    cout << "Done." << endl;

}
