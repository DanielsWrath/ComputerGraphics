//
//  Framework for a raytracer
//  File: obj.h
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

#ifndef OBJ_H_115209AE
#define OBJ_H_115209AE

#include "object.h"
#include "triangle.h"
#include "glm.h"

class Obj : public Object
{
public:
    Obj(GLMmodel *model, Point position) : model(model), position(position) { }
    virtual Hit intersect(const Ray &ray);
    virtual Point getTexturePoint(const Point &p);
    Point getVertexPoint(int vertexIndex);
    Triangle* parseObjTriangle(GLMtriangle *t);
    const GLMmodel *model;
    const Point position;
};

#endif /* end of include guard: CYLINDER_H_115209AE */
