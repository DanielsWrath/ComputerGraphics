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
#include <math.h>
#include <stdio.h>
#include <cstring>

#define PI 3.14159265359

// Returns pointer to object if the ray intersects an object, else returns NULL
Object* Scene::traceObject(const Ray &ray){
    Hit min_hit(std::numeric_limits<double>::infinity(),Vector());
    Object *obj = NULL;
    for (unsigned int i = 0; i < objects.size(); ++i) {
        Hit hit(objects[i]->intersect(ray));
        if (hit.t<min_hit.t) {
            min_hit = hit;
            obj = objects[i];
        }
    }
    return obj;
}

Color Scene::trace(const Ray &ray, int recursion, bool gooch)
{
    //If recursion depth is higher than the max recursion depth, return black
    if(recursion > maxRecursionDepth){
        return Color(0,0,0);
    }
	Color finalColor = Color(0,0,0);
    
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
    // If object uses texture, get UV coordinates and get color of the texture at pos UV
    if (material->useTexture == true) {
        Point UV = obj->getTexturePoint(hit);
        color = textures[material->texturePos]->colorAt(UV.x, UV.y);
    }
    finalColor += color * material->ka;
    // loop through all lights.
    for (unsigned int i = 0; i < lights.size() ; i++ ){
        Light *currentLight = lights[i]; 
        Vector L = (currentLight->position - hit).normalized(); // Light vector
        Vector R =2*(N.dot(L))*N - L; // Opposite of L
        Vector H = (L + V)/ (L + V).length(); // between L and V
        // Calculate diffuse and specular shaders
        double D = material->kd * max(N.dot(L),(double)0);
        double S = material->ks * max(pow(N.dot(H), 3*material->n),(double)0); // Blinn-Phong using H vector
        //double S = material->ks * max(pow(V.dot(R), material->n), (double)0); // THIS CAUSES WEIRD SPECULARS
        // If diffuse is smaller than 0, set S to 0
        if (D < 0) {
            S = 0;
        }
        // If shadows must be created, trace ray from hitpoint to light source. If it returns an object, set D to 0
        if(renderShadows == true){
            Object* objectHit = traceObject(Ray(hit, L));
            if (objectHit){
                D = 0;
                S = 0;
            }
        }

		// If gooch is used, set D to 0, calculate the gooch shading and use the phong specular
        if (gooch) {
            D = 0;
            Triple kd = currentLight->color*material->color*material->kd;
            Triple kCool = Triple(0,0,b)+alpha*kd, kWarm = Triple(y,y,0) + beta*kd;
            Triple I = kCool *(1 - N.dot(L))/2 + kWarm * (1 + N.dot(L))/2;
            finalColor += I + S*currentLight->color;
            /* double specular = pow(max(0, R.dot(V)), material->n) * material->ks;*/
        }else{
        //Use phong shading
            finalColor += ( currentLight->color * color*D) + S*currentLight->color;
        }
        
    }
    //Calculate viewvector reflected
    Vector viewReflect = 2*(N.dot(V))*N - V;
    //Add recursive reflection to color
    return finalColor  + material->ks * trace(Ray(hit, viewReflect), recursion+1, gooch);
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

// Default renderer (Phong), has a bool variabl gooch, is set true if said shading is used
void Scene::render(Image &img, bool gooch)
{
	// add textures which are needed
	addImages();
    Vector V, H, xInc, yInc;
    Point bottomLeft;
    // ssAdd is the factor which needs to be added to the pixel point, needed for SuperSampling (if no SS is used it is set to 1/2)
    double ssAdd = 1/((double)superSampling+1);
    int w = img.width();
    int h = img.height();
    
    // If a camera is given calculate the needed vectors
    if (cameraUsed == true) {
        Vector viewDirection = (center - eye);
        // calculate right and up vectors
        H = (viewDirection.cross(up));
        V = H.cross(viewDirection);
        H.normalize();
        V.normalize();
        // Instead of FOV, w/2 and h/2 is used
        double viewPlaneHalfWidth = w/(2/up.length());// tanf(PI/4);
        double viewPlaneHalfHeight = h/(2/up.length()); // (h/w)*viewPlaneHalfHeight;
        // Calculate bottom left corner
        bottomLeft = center - V*viewPlaneHalfHeight - H*viewPlaneHalfWidth;
        // Calculate vectors, which will be multiplied by x and y
        xInc =  (H*2*viewPlaneHalfWidth)/w;
        yInc = (V*2*viewPlaneHalfHeight)/h;
    }
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            //initialize color of pixel in image
            Color totalColor = Color(0,0,0);
            //loop again for supersampling^2 times (once for x once for y), default is 1
            for ( int ssY = 0 ; ssY < superSampling; ssY++) {
                for ( int ssX = 0; ssX < superSampling; ssX++) {
                    //If camera is used, use xInc and yInc vectors to calculate point
                    if (cameraUsed == true) {
                        Point pixelCam = bottomLeft + (x) *xInc+ (h-1-y)*yInc;
                        // Add supersampling offset
                        pixelCam.x = pixelCam.x +(ssAdd*(ssX+1));
                        pixelCam.y = pixelCam.y + (ssAdd*(ssY+1));
                        Ray ray(eye, (pixelCam-eye).normalized());
                        totalColor += trace(ray,1, gooch);
                    }else{
                        //If just an eye is given, use default calculations. Also supersampling offset is used
                        Point pixel(x+ ssAdd*(ssX+1), h-1-y+ ssAdd*(ssY+1),0);
                        Ray ray(eye, (pixel-eye).normalized());
                        totalColor += trace(ray,1, gooch);
                    }
                }
            }
            // Devide the eventual color by supersampling^2 (default is 1)
            totalColor /= (superSampling*superSampling);
            totalColor.clamp();
            img(x,y) = totalColor;
        }
    }
    // delete texture array
    delete[] textures;
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

// load image using a string
Image* Scene::loadImage(string file){
	return new Image(file.c_str());
}

// add images to textures array
void Scene::addImages(){
    cout << "Loading Textures..." << endl;
    const int count = objects.size();
    // create string array of objects size
    string fileNames[count];
    unsigned int maxTex = 0;
    // loop through objects
    for (unsigned int i = 0; i < objects.size(); ++i){
        Object *obj = objects[i];
        Material *m = obj->material;
        // if objects use texture check if we already stored that texture in the string array
        // if not, add it. 
        if (m->useTexture == true) {
            for (unsigned int j = 0; j<=maxTex; j++) {
                if (fileNames[j] == m->texture) {
                    // store position of texture array
                    m->texturePos = j;
                    break;
                }
                else if(j == maxTex){
					// add texturename to string array and store position
                    fileNames[j] = m->texture;
                    m->texturePos = j;
                    maxTex++;
                    break;
                }
            }
        }
    }
    // allocate memory to textures 
    textures = new Image*[maxTex];
    //load needed textures
    for (unsigned int i = 0; i < maxTex; i++) {
        textures[i] = loadImage(fileNames[i]);
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

void Scene::setRenderShadows(bool shadows){
	renderShadows = shadows;
}

void Scene::setMaxRecursionDepth(int i){
	maxRecursionDepth = i;
}

void Scene::setSuperSampling(int i){
    superSampling = i;
}

//Set true if camera is used, false otherwise
void Scene::useCamera(bool use){
    cameraUsed = use;
}

// Set camera variables
void Scene::setCamera(Triple eyeIn, Triple centerIn, Triple upIn){
    eye = eyeIn;
    center = centerIn;
    up = upIn;
}

void Scene::setGooch(float bIn, float yIn, float alphaIn, float betaIn){
    b = bIn;
    y = yIn;
    alpha = alphaIn;
    beta = betaIn;
}


// See if a camera is used
bool Scene::getUseCamera(){
    return cameraUsed;
}
