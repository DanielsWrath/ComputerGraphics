//
//  Framework for a raytracer
//  File: scene.h
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

#ifndef SCENE_H_KNBLQLP6
#define SCENE_H_KNBLQLP6

#include <vector>
#include "triple.h"
#include "light.h"
#include "object.h"
#include "image.h"

class Scene
{
private:

    float b,y,alpha,beta; // Gooch parameters
    std::vector<Object*> objects;
    std::vector<Light*> lights;
    Triple eye, center, up; // camera parameters
    bool renderShadows, cameraUsed;
    int maxRecursionDepth, superSampling; // recursion and ss parameters
    Object* traceObject(const Ray &ray);
    Image **textures; // array of pointers to texture images
    void addImages();
    Image* loadImage(string file);
public:
    Color trace(const Ray &ray, int recursion, bool gooch);
    Color traceZ(const Ray &ray);
    Color traceNormal(const Ray &ray);
    void render(Image &img, bool gooch);
    void renderZ(Image &img);
    void renderNormal(Image &img);
    void addObject(Object *o);
    void addLight(Light *l);
    void setEye(Triple e);
    unsigned int getNumObjects() { return objects.size(); }
    unsigned int getNumLights() { return lights.size(); }
    void setRenderShadows(bool shadows);
    void setMaxRecursionDepth(int i);
    void setSuperSampling(int i);
    void useCamera(bool use);
    void setCamera(Triple eyeIn, Triple centerIn, Triple upIn);
    void setGooch(float bIn, float yIn, float alphaIn, float betaIn);
    bool getUseCamera();
    
};

#endif /* end of include guard: SCENE_H_KNBLQLP6 */
