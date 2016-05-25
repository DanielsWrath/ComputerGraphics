//
//  Framework for a raytracer
//  File: obj.cpp
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

#include "obj.h"
#include "triangle.h"
#include <stdio.h>
#include <iostream>
#include <math.h>

#define PI 3.1415926
#define LAMBDA 0.00001

/************************** Obj **********************************/
Point Obj::getVertexPoint(int vertexIndex) {
    return Point(model->vertices[vertexIndex*3],
                 model->vertices[vertexIndex*3+1],
                 model->vertices[vertexIndex*3+2]) + position;
    
}

Triangle* Obj::parseObjTriangle(GLMtriangle *t){
    Point p1 = getVertexPoint(t->vindices[0]), p2= getVertexPoint(t->vindices[1]), p3= getVertexPoint(t->vindices[2]);
   // cout << p1 << endl;
   // cout << p2 << endl;
   // cout << p3 << endl;
    return new Triangle(p3,p1,p2);
}

Hit Obj::intersect(const Ray &ray)
{
    Hit min_hit(std::numeric_limits<double>::infinity(),Vector());
    Triangle *min_t = NULL;
    for (unsigned i = 0; i < model->numtriangles; i++) {
        Triangle *t = parseObjTriangle(&(model->triangles[i]));
        //cout << t->position0 << endl;
        Hit hit(t->intersect(ray));
        //cout << hit.t << endl;
        if (hit.t<min_hit.t) {
            min_hit = hit;
            min_t =  t;
        }
        
    }
    if (min_t == NULL) {
        return Hit::NO_HIT();
    }
    //cout << "HIT OBJ" << endl;

    return min_hit;
}

Point Obj::getTexturePoint(const Point &p){
    return Point(-1,-1,-1);
}
