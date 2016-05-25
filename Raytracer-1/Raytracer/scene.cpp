//
//  Framework for a raytracer
//  File: scene.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Maarten Everts
//    Jasper van de Gronde
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#include "scene.h"
#include "material.h"
#include <stdio.h>

Color Scene::trace(const Ray &ray)
{
    // Find hit object and distance
    Hit min_hit(std::numeric_limits<double>::infinity(),Vector());
    Object *obj = NULL;
    for (unsigned int i = 0; i < objects.size(); ++i) {
        Hit hit(objects[i]->intersect(ray));
        if (hit.t<min_hit.t) {
            min_hit = hit;
            obj = objects[i];
        }
    }

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);

    Material *material = obj->material;            //the hit objects material
    Point hit = ray.at(min_hit.t);                 //the hit point
    Vector N = min_hit.N;                          //the normal at hit point
    Vector V = -ray.D;                             //the view vector
    Color color = material -> color;


    /****************************************************
    * This is where you should insert the color
    * calculation (Phong model).
    *
    * Given: material, hit, N, V, lights[]
    * Sought: color
    *
    * Hints: (see triple.h)
    *        Triple.dot(Vector) dot product
    *        Vector+Vector      vector sum
    *        Vector-Vector      vector difference
    *        Point-Point        yields vector
    *        Vector.normalize() normalizes vector, returns length
    *        double*Color        scales each color component (r,g,b)
    *        Color*Color        dito
    *        pow(a,b)           a to the power of b
    ****************************************************/

// loop through all lights.
    for (unsigned int i = 0; i < lights.size() ; i++ ){
        Light *currentLight = lights[i]; 
        //Point lightPos = currentLight->position; 
        Vector L = (currentLight->position - hit).normalized(); // Light vector
        //Vector R = 2*(N.dot(L))*N - L; // Opposite of L
        Vector H = (L + V)/ (L + V).length(); // between L and V
        // Calculate all shaders and combine them into I
        double A = material->ka; 
        double D = material->kd * max(N.dot(L),(double)0);
        double S =  material->ks * pow(N.dot(H), material->n); // S
        double att = 1; // d should be divided by r, the radius of influence of the light, but this is not present in light.h
        double I =A + att*(D + S);

        //Color set
        color = color*I*currentLight->color;
    }

    return color;
}

// trace the Z buffer. Normal intersection calculations are done, but the distance t is normalized over a maxZ. This is then set as the color for the point.
Color Scene::traceZ(const Ray &ray){
    double maxZ = 1500;
    double distance = 0;
    Color color;

    // Find hit object and distance
    Hit min_hit(std::numeric_limits<double>::infinity(),Vector());
    Object *obj = NULL;
    for (unsigned int i = 0; i < objects.size(); ++i) {
        Hit hit(objects[i]->intersect(ray));
        if (hit.t<min_hit.t) {
            min_hit = hit;
            distance = hit.t;
            obj = objects[i];
        }
    }

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);

	//normalize distance
    color.set((maxZ-distance)/maxZ);
    return color;
}

// Trace the Normal buffer. shows the calculated normals in the intersection function of the objects and shows them as colors (x,y,z) to (r,g,b)
Color Scene::traceNormal(const Ray &ray){
    Color color;

    // Find hit object and distance
    Hit min_hit(std::numeric_limits<double>::infinity(),Vector());
    Object *obj = NULL;
    for (unsigned int i = 0; i < objects.size(); ++i) {
        Hit hit(objects[i]->intersect(ray));
        if (hit.t<min_hit.t) {
            min_hit = hit;
            obj = objects[i];
        }
    }

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);

	// Set normal to a range of 0 - 1 instead of -1 - 1
    Vector normal = (min_hit.N +1) / 2;
    color.set(normal.x, normal.y, normal.z);
    return color;
}

// Default renderer (Phong)
void Scene::render(Image &img)
{
    int w = img.width();
    int h = img.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x+0.5, h-1-y+0.5, 0);
            Ray ray(eye, (pixel-eye).normalized());
            Color col = trace(ray);
            col.clamp();
            img(x,y) = col;
        }
    }
}
// Z buffer renderer
void Scene::renderZ(Image &img){
	int w = img.width();
    int h = img.height();
	for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x+0.5, h-1-y+0.5, 0);
            Ray ray(eye, (pixel-eye).normalized());
            Color col = traceZ(ray);
            col.clamp();
            img(x,y) = col;
        }
    }
}
// Normal buffer renderer.
void Scene::renderNormal(Image &img){
	int w = img.width();
    int h = img.height();
	for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x+0.5, h-1-y+0.5, 0);
            Ray ray(eye, (pixel-eye).normalized());
            Color col = traceNormal(ray);
            col.clamp();
            img(x,y) = col;
        }
    }
}


void Scene::addObject(Object *o)
{
    objects.push_back(o);
}

void Scene::addLight(Light *l)
{
    lights.push_back(l);
}

void Scene::setEye(Triple e)
{
    eye = e;
}
