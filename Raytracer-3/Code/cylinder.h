//
//  Framework for a raytracer
//  File: sphere.h
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

#ifndef CYLINDER_H_115209AE
#define CYLINDER_H_115209AE

#include "object.h"

class Cylinder : public Object
{
public:
    Cylinder(Point position0,Point position1, double r) : position0(position0), position1(position1), r(r) { }
    virtual Hit intersect(const Ray &ray);
    virtual Point getTexturePoint(const Point &p);
    const Point position0;
    const Point position1;
    const double r;
};

#endif /* end of include guard: CYLINDER_H_115209AE */
